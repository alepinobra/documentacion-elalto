# Flujo de Procesamiento de Mensajes

## Diagrama de Flujo General

```mermaid
flowchart TD
    Start([Mensaje recibido en Webhook]) --> CheckDirection{¿Dirección<br/>inbound?}
    
    CheckDirection -->|No| IgnoreOutbound[Ignorar mensaje<br/>outbound/reaction]
    IgnoreOutbound --> End1([Fin - 200 OK])
    
    CheckDirection -->|Sí| HasConvId{¿Tiene<br/>conversationId?}
    
    HasConvId -->|No| WarnNoConv[⚠️ Advertencia:<br/>Sin conversationId]
    WarnNoConv --> End2([Fin - 200 OK])
    
    HasConvId -->|Sí| CheckLinking{¿Vinculación<br/>en progreso?}
    
    CheckLinking -->|Sí| HandleLinking[Procesar vinculación<br/>RUT/Teléfono]
    HandleLinking --> ValidateLinking{¿Vinculación<br/>válida?}
    
    ValidateLinking -->|No| SendLinkingError[Enviar error<br/>de validación]
    SendLinkingError --> End3([Fin - 200 OK])
    
    ValidateLinking -->|Sí| UpdateUser[Actualizar usuario<br/>en DB]
    UpdateUser --> SendSuccess[Enviar confirmación]
    SendSuccess --> End4([Fin - 200 OK])
    
    CheckLinking -->|No| CheckUser{¿Usuario<br/>existe?}
    
    CheckUser -->|No| SearchByPhone[Buscar por<br/>teléfono]
    SearchByPhone --> UserFound{¿Usuario<br/>encontrado?}
    
    UserFound -->|No| StartLinking[Iniciar proceso<br/>de vinculación]
    StartLinking --> RequestRut[Solicitar RUT<br/>al usuario]
    RequestRut --> End5([Fin - 200 OK])
    
    UserFound -->|Sí| UpdateConvId[Actualizar<br/>conversationId]
    UpdateConvId --> ProcessMsg[Continuar procesamiento]
    
    CheckUser -->|Sí| ProcessMsg
    
    ProcessMsg --> ContentFilter[Content Filter]
    ContentFilter --> FilterResult{¿Contenido<br/>bloqueado?}
    
    FilterResult -->|Sí| SendWarning[Enviar mensaje<br/>de advertencia]
    SendWarning --> End6([Fin - 200 OK])
    
    FilterResult -->|No| Guardrails[Verificar Guardrails]
    Guardrails --> GuardrailPass{¿Pasa<br/>guardrails?}
    
    GuardrailPass -->|No| SendSafeResponse[Enviar respuesta<br/>segura filtrada]
    SendSafeResponse --> End7([Fin - 200 OK])
    
    GuardrailPass -->|Sí| CheckAdmin{¿Usuario<br/>Admin?}
    
    CheckAdmin -->|Sí| AdminFlow[Admin Orchestrator]
    CheckAdmin -->|No| UserFlow[User Orchestrator]
    
    AdminFlow --> ExecuteQuery[Ejecutar consulta<br/>sin restricciones]
    UserFlow --> ApplySecurity[Aplicar filtros<br/>de seguridad]
    ApplySecurity --> ExecuteQuery
    
    ExecuteQuery --> NeedsDB{¿Necesita<br/>base de datos?}
    
    NeedsDB -->|No| DirectResponse[Respuesta directa<br/>del agente]
    DirectResponse --> FormatResponse
    
    NeedsDB -->|Sí| SelectAgent[Seleccionar agente<br/>especializado]
    SelectAgent --> GenerateSQL[Generar consulta SQL]
    GenerateSQL --> ExecuteSQL[Ejecutar SQL via MCP]
    ExecuteSQL --> ProcessResults[Procesar resultados]
    ProcessResults --> FormatResponse[Formatear respuesta]
    
    FormatResponse --> OutputGuardrails[Guardrails de salida]
    OutputGuardrails --> SendResponse[Enviar respuesta<br/>a WhatsApp]
    SendResponse --> End8([Fin - 200 OK])
    
    style Start fill:#25D366
    style End1 fill:#90EE90
    style End2 fill:#90EE90
    style End3 fill:#90EE90
    style End4 fill:#90EE90
    style End5 fill:#90EE90
    style End6 fill:#90EE90
    style End7 fill:#90EE90
    style End8 fill:#90EE90
    style SendWarning fill:#FFA500
    style SendLinkingError fill:#FFA500
    style ContentFilter fill:#FFD700
    style Guardrails fill:#FFD700
```

## Fases del Procesamiento

### Fase 1: Recepción y Validación Inicial

```mermaid
sequenceDiagram
    participant K as Kapso API
    participant W as Webhook Router
    participant L as Logger
    
    K->>W: POST / con mensaje
    W->>L: Log timestamp
    W->>L: Log body completo
    W->>W: Extraer message
    W->>W: Extraer conversationId
    W->>L: Log tipo y dirección
    
    alt Mensaje no es inbound
        W->>L: Log ignorar outbound
        W->>K: 200 OK
    else Es inbound
        alt Sin conversationId
            W->>L: Warning sin conversationId
            W->>K: 200 OK
        else Con conversationId
            W->>W: Continuar procesamiento
        end
    end
```

### Fase 2: Autenticación y Vinculación

```mermaid
sequenceDiagram
    participant W as Webhook
    participant CM as Conversation Memory
    participant AUTH as Auth Service
    participant DB as Database
    participant U as Usuario WhatsApp
    
    W->>CM: isAwaitingAccountLinking(convId)
    
    alt Vinculación en progreso
        CM-->>W: true
        W->>W: Extraer RUT del mensaje
        W->>AUTH: validateRutForLinking()
        
        alt RUT inválido
            AUTH-->>W: Error validación
            W->>U: Mensaje error formato
        else RUT válido
            AUTH->>DB: Buscar usuario por RUT
            alt Usuario no existe
                DB-->>AUTH: null
                AUTH-->>W: Error usuario no existe
                W->>U: Usuario no encontrado
            else Usuario existe
                DB-->>AUTH: userData
                AUTH->>DB: updateConversationId()
                DB-->>AUTH: actualizado
                AUTH->>CM: clearLinkingProcess()
                AUTH-->>W: Vinculación exitosa
                W->>U: ✅ Cuenta vinculada
            end
        end
    else Sin vinculación pendiente
        CM-->>W: false
        W->>AUTH: getUserInfo(convId)
        
        alt Usuario no existe
            AUTH-->>W: null
            W->>DB: getUserByPhone()
            
            alt Encontrado por teléfono
                DB-->>W: userData
                W->>DB: updateConversationId()
                W->>W: Continuar procesamiento
            else No encontrado
                W->>CM: setAwaitingAccountLinking()
                W->>U: Solicitar RUT para vincular
            end
        else Usuario existe
            AUTH-->>W: userData
            W->>W: Continuar procesamiento
        end
    end
```

### Fase 3: Filtrado y Seguridad

```mermaid
sequenceDiagram
    participant W as Webhook
    participant CF as Content Filter
    participant GR as Guardrails
    participant OAI as OpenAI API
    participant O as Orchestrator
    
    W->>CF: shouldBlockMessage(text)
    CF->>CF: Verificar palabras prohibidas
    CF->>CF: Detectar solicitudes de código
    
    alt Contenido bloqueado
        CF-->>W: true + warning message
        W->>W: Enviar advertencia
    else Contenido OK
        CF-->>W: false
        W->>GR: checkGuardrails(text, isInput=true)
        GR->>OAI: moderations.create()
        OAI-->>GR: moderation result
        
        alt Moderación falla
            GR->>GR: Generar respuesta segura
            GR-->>W: passed=false + safeText
            W->>W: Enviar respuesta segura
        else Moderación OK
            GR-->>W: passed=true + safeText
            W->>O: Enviar a orchestrator
        end
    end
```

### Fase 4: Orquestación y Ejecución

```mermaid
sequenceDiagram
    participant W as Webhook
    participant O as Orchestrator
    participant AF as Agent Factory
    participant A as Agent IA
    participant MCP as MCP Client
    participant DB as SQL Server
    
    W->>O: runUserWorkflow() / runAdminWorkflow()
    O->>O: Obtener userInfo
    O->>O: Construir security context
    O->>O: Analizar mensaje
    
    alt Necesita DB
        O->>AF: selectAgent(message, isAdmin)
        AF->>AF: Analizar keywords
        AF-->>O: Agent especializado
        
        O->>A: Fase 1: Generar SQL
        A->>A: Procesar con contexto
        A-->>O: SQL query
        
        O->>MCP: executeSQL(query)
        MCP->>DB: Ejecutar query
        DB-->>MCP: Resultados
        MCP-->>O: Data
        
        O->>O: Formatear con Toon
        O->>A: Fase 2: Formatear respuesta
        A->>A: Generar respuesta en lenguaje natural
        A-->>O: Respuesta formateada
    else No necesita DB
        O->>A: Procesar mensaje
        A-->>O: Respuesta directa
    end
    
    O->>GR: checkGuardrails(output, isInput=false)
    GR-->>O: Respuesta filtrada
    O-->>W: Respuesta final
    W->>W: Enviar a WhatsApp
```

## Manejo de Casos Especiales

### Menú Interactivo

```mermaid
stateDiagram-v2
    [*] --> DetectMenuRequest: Usuario dice "menú" o "opciones"
    DetectMenuRequest --> CheckUserType: Identificar tipo
    
    CheckUserType --> AdminMenu: Usuario Admin
    CheckUserType --> UserMenu: Usuario Normal
    
    AdminMenu --> SendAdminButtons: Botones administrativos
    UserMenu --> SendUserButtons: Botones usuarios
    
    SendAdminButtons --> WaitSelection: Esperar interacción
    SendUserButtons --> WaitSelection
    
    WaitSelection --> ProcessButton: Usuario presiona botón
    ProcessButton --> ExecuteAction: Ejecutar acción
    ExecuteAction --> [*]
```

### Solicitud de Parámetros

```mermaid
sequenceDiagram
    participant U as Usuario
    participant W as Webhook
    participant PA as Parametros Agent
    participant CM as Conversation Memory
    
    U->>W: "Quiero ver mis viajes"
    W->>PA: analyzeAndRequestParameters()
    PA->>PA: Detectar parámetros faltantes
    
    alt Faltan parámetros
        PA->>CM: Guardar contexto de consulta
        PA-->>W: Mensaje solicitud parámetros
        W->>U: "¿De qué fecha a qué fecha?"
        
        U->>W: "Del 1 al 31 de diciembre"
        W->>CM: Obtener contexto guardado
        CM-->>W: Consulta original
        W->>PA: Combinar consulta + parámetros
        PA-->>W: Consulta completa
        W->>W: Ejecutar workflow normal
    else Parámetros completos
        PA-->>W: Continuar con consulta
        W->>W: Ejecutar workflow normal
    end
```

## Respuestas Asíncronas

El webhook siempre responde inmediatamente con 200 OK a Kapso, luego procesa el mensaje de forma asíncrona:

```javascript
// Respuesta inmediata
res.status(200).end();

// Procesamiento asíncrono
(async () => {
  try {
    const response = await processMessage();
    await sendWhatsAppMessage(conversationId, response);
  } catch (error) {
    console.error('Error:', error);
    await sendWhatsAppMessage(conversationId, '❌ Error procesando mensaje');
  }
})();
```

## Logs y Debugging

Cada fase del procesamiento genera logs estructurados:

```
[DEPURACIÓN] Webhook recibido 2025-12-26 10:30:00
[DEPURACIÓN] 📦 Body completo: {...}
[DEPURACIÓN] 📨 Tipo de mensaje: text
[DEPURACIÓN] 📨 Dirección: inbound
[User Orchestrator] 🔍 Iniciando flujo de usuario...
[User Orchestrator] 🤖 AGENTE SELECCIONADO: Document Agent
[MCP Client] Ejecutando SQL query...
[DEPURACIÓN] ✅ Respuesta enviada
```

## Próximos Pasos

- [Sistema de Agentes](./sistema-agentes.md)
- [Seguridad y Validación](./seguridad.md)
- [MCP y Base de Datos](./mcp-database.md)

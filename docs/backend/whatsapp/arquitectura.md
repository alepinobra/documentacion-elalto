# Webhook WhatsApp - Arquitectura

## Descripción General

Sistema de mensajería inteligente basado en agentes de IA que permite a los usuarios interactuar con Tracmin a través de WhatsApp, implementando arquitectura de agentes orquestados con acceso a base de datos mediante Model Context Protocol (MCP).

??? info "Notas Importantes"
    - Arquitectura basada en agentes especializados (Factory Pattern)
    - Multi-capa de seguridad: Content Filter + Guardrails + Validación RUT
    - Model Context Protocol para acceso seguro a SQL Server
    - Conversación natural con formato Toon
    - Roles diferenciados: Usuario Normal y Administrador
    - Memoria de conversación persistente por usuario
    - Autenticación robusta con vinculación de cuentas

??? warning "Consideraciones Importantes"
    - Requiere cuenta de Meta Business y WhatsApp Business API
    - El webhook debe ser público y accesible desde internet
    - Validación de token en cada request de Meta
    - Los mensajes son procesados de forma asíncrona
    - Timeout de 30 segundos para respuestas de agentes
    - Rate limiting configurable por usuario

=== "Stack Tecnológico"
    ## Tecnologías Core

    | Tecnología | Versión | Propósito |
    |------------|---------|-----------|
    | **Node.js** | 18+ | Runtime JavaScript |
    | **Express.js** | 4.21.2 | Framework web |
    | **TypeScript** | 5.0+ | Lenguaje tipado |
    | **@kapso/whatsapp-cloud-api** | 0.1.1 | Cliente WhatsApp Cloud API |

    ## IA y Agentes

    | Tecnología | Versión | Propósito |
    |------------|---------|-----------|
    | **@openai/agents** | latest | Orquestación de agentes IA |
    | **@openai/guardrails** | latest | Filtrado de contenido y seguridad |
    | **@toon-format/toon** | 0.9.0 | Formato natural de respuestas |

    ## Base de Datos y Protocolos

    | Tecnología | Versión | Propósito |
    |------------|---------|-----------|
    | **@modelcontextprotocol/sdk** | 1.0.4 | Protocolo MCP para BD |
    | **mssql** | 11.0.1 | Driver SQL Server |

    ## Utilidades

    | Tecnología | Versión | Propósito |
    |------------|---------|-----------|
    | **node-fetch** | 3.3.2 | Cliente HTTP |
    | **dotenv** | 16.4.7 | Variables de entorno |

=== "Diagrama Simplificado"
    ```mermaid
    graph LR
        A[Express.js] --> B[OpenAI Agents]
        B --> C[MCP Protocol]
        C --> D[SQL Server]
        A --> E[Kapso WhatsApp API]
        B --> F[Guardrails]
        B --> G[Toon Formatter]
        
        style A fill:#68A063
        style B fill:#74AA9C
        style D fill:#CC2927
        style E fill:#25D366
    ```

## Arquitectura del Sistema

=== "Vista General"
    ```mermaid
    graph TB
        subgraph "Cliente WhatsApp"
            WA[Usuario WhatsApp]
        end
        
        subgraph "API Gateway"
            KA[Kapso API<br/>WhatsApp Cloud]
        end
        
        subgraph "Webhook Server"
            WH[Express Webhook<br/>POST /]
            VT[Verificación Token<br/>GET /]
            RT[Router]
        end
        
        subgraph "Capa de Seguridad"
            CF[Content Filter]
            GR[Guardrails]
            RV[RUT Validator]
            AUTH[Auth Service]
        end
        
        subgraph "Capa de Orquestación"
            UO[User Orchestrator]
            AO[Admin Orchestrator]
            PA[Parametros Agent]
        end
        
        subgraph "Capa de Agentes IA"
            UA[User Agent]
            AA[Admin Agent]
            subgraph "Database Agents"
                DOC[Document Agent]
                FUEL[Fuel Agent]
                OP[Operation Agent]
                VEH[Vehicle Agent]
                TRP[Transporter Agent]
            end
        end
        
        subgraph "Capa de Datos"
            MCP[MCP Client]
            MCPS[MCP Server]
            DB[(SQL Server<br/>Tracmin DB)]
        end
        
        subgraph "Utilidades"
            CM[Conversation Memory]
            MF[Message Formatter]
            TF[Toon Formatter]
            IM[Interactive Messages]
        end
        
        WA --> KA
        KA --> WH
        WH --> RT
        RT --> CF
        CF --> GR
        GR --> RV
        RV --> AUTH
        AUTH --> UO
        AUTH --> AO
        
        UO --> PA
        AO --> PA
        
        UO --> UA
        AO --> AA
        
        UA --> DOC
        UA --> FUEL
        UA --> OP
        UA --> VEH
        UA --> TRP
        
        AA --> DOC
        AA --> FUEL
        AA --> OP
        AA --> VEH
        AA --> TRP
        
        DOC --> MCP
        FUEL --> MCP
        OP --> MCP
        VEH --> MCP
        TRP --> MCP
        
        MCP --> MCPS
        MCPS --> DB
        
        CM -.-> UO
        CM -.-> AO
        MF -.-> RT
        TF -.-> UO
        TF -.-> AO
        IM -.-> RT
        
        style WA fill:#25D366
        style KA fill:#128C7E
        style DB fill:#CC2927
        style GR fill:#FFA500
        style AUTH fill:#FF6B6B
    ```

=== "Capas del Sistema"
    ## 1. Capa de Cliente
    - Usuario final en WhatsApp
    - Envía mensajes de texto
    - Recibe respuestas formateadas

    ## 2. Capa de Gateway
    - Kapso WhatsApp Cloud API
    - Gestión de mensajes entrantes/salientes
    - Webhook de Meta

    ## 3. Capa de Seguridad
    - **Content Filter:** Filtrado inicial de contenido
    - **Guardrails:** Validación de seguridad IA
    - **RUT Validator:** Validación de RUT chileno
    - **Auth Service:** Autenticación y autorización

    ## 4. Capa de Orquestación
    - **User Orchestrator:** Orquestador para usuarios normales
    - **Admin Orchestrator:** Orquestador para administradores
    - **Parametros Agent:** Extracción de parámetros

    ## 5. Capa de Agentes
    - **User/Admin Agent:** Agentes principales
    - **Database Agents:** Agentes especializados por dominio
      - Documentos
      - Combustible
      - Operaciones
      - Vehículos
      - Transportistas

    ## 6. Capa de Datos
    - **MCP Client/Server:** Protocolo de contexto de modelo
    - **SQL Server:** Base de datos Tracmin

=== "Flujo de Procesamiento"
    ```mermaid
    sequenceDiagram
        participant U as Usuario WhatsApp
        participant K as Kapso API
        participant W as Webhook
        participant CF as Content Filter
        participant GR as Guardrails
        participant AUTH as Auth Service
        participant O as Orchestrator
        participant A as Agent IA
        participant MCP as MCP Server
        participant DB as SQL Server
        
        U->>K: Enviar mensaje
        K->>W: POST / (webhook)
        W->>CF: Validar contenido
        CF->>GR: Aplicar guardrails
        GR->>AUTH: Verificar usuario
        
        alt Usuario no registrado
            AUTH->>W: Solicitar vinculación
            W->>K: Enviar instrucciones
            K->>U: Mensaje vinculación
        else Usuario registrado
            AUTH->>O: Mensaje + Contexto
            O->>A: Procesar con IA
            A->>MCP: Query datos
            MCP->>DB: SELECT
            DB-->>MCP: Resultado
            MCP-->>A: Datos
            A-->>O: Respuesta IA
            O->>W: Formatear respuesta
            W->>K: Enviar mensaje
            K->>U: Respuesta final
        end
    ```
    participant MCP as MCP Client
    participant DB as SQL Server
    
    U->>K: Envía mensaje
    K->>W: POST / webhook event
    
    W->>W: Verificar dirección (inbound)
    W->>CF: Filtrar contenido
    
    alt Contenido bloqueado
        CF-->>W: Mensaje de advertencia
        W->>K: Enviar advertencia
        K->>U: Mostrar advertencia
    else Contenido válido
        CF->>GR: Verificar guardrails
        
        alt Guardrails no pasa
            GR-->>W: Respuesta segura
            W->>K: Enviar respuesta
            K->>U: Mostrar respuesta
        else Guardrails OK
            GR->>AUTH: Validar usuario
            
            alt Usuario no existe
                AUTH-->>W: Iniciar vinculación
                W->>K: Solicitar RUT/Teléfono
                K->>U: Formulario vinculación
            else Usuario existe
                AUTH->>AUTH: Verificar tipo (Admin/User)
                
                alt Usuario Admin
                    AUTH->>O: runAdminWorkflow()
                else Usuario Normal
                    AUTH->>O: runUserWorkflow()
                end
                
                O->>A: Ejecutar agente IA
                A->>MCP: Consulta SQL
                MCP->>DB: Ejecutar query
                DB-->>MCP: Resultados
                MCP-->>A: Datos
                A->>A: Procesar y formatear
                A-->>O: Respuesta formateada
                O-->>W: Respuesta final
                W->>K: Enviar mensaje
                K->>U: Mostrar respuesta
            end
        end
    end
```

## Componentes Principales

=== "Express Webhook"
    ### 1. Express Webhook (`app.js`, `webhook.js`)

    Punto de entrada del sistema que recibe eventos de WhatsApp.

    **Endpoints:**

    | Método | Ruta | Descripción |
    |--------|------|-------------|
    | `GET` | `/` | Verificación de webhook (handshake con Kapso) |
    | `POST` | `/` | Recepción de mensajes y eventos |

    ??? info "Flujo de Verificación"
        ```javascript
        // GET / - Verificación
        app.get('/', (req, res) => {
          const mode = req.query['hub.mode'];
          const token = req.query['hub.verify_token'];
          const challenge = req.query['hub.challenge'];
          
          if (mode && token === VERIFY_TOKEN) {
            res.status(200).send(challenge);
          } else {
            res.sendStatus(403);
          }
        });
        ```

=== "Capa de Seguridad"
    ### 2. Capa de Seguridad

    #### Content Filter (`contentFilter.js`)
    Bloquea solicitudes relacionadas con trabajo o código malicioso.

    ??? warning "Filtros Activos"
        - Solicitudes de código
        - Peticiones de empleo
        - Contenido malicioso
        - Inyección SQL
        - Scripts maliciosos

    #### Guardrails (`guardrails.js`)
    Valida entrada/salida usando OpenAI Moderation API y filtros personalizados.

    ??? info "Validaciones"
        **Input Guardrails:**
        - Moderación de contenido con OpenAI
        - Filtrado de patrones sospechosos
        - Validación de intenciones

        **Output Guardrails:**
        - Verificación de respuestas IA
        - Filtrado de información sensible
        - Validación de formato

    #### RUT Validator (`rutValidator.js`)
    Valida identidad chilena (RUT) y vinculación de cuentas.

    #### Auth Service (`authService.js`)
    Gestiona autenticación y permisos de usuarios.

=== "Orquestadores"
    ### 3. Orquestadores

    #### User Orchestrator (`userOrchestrator.js`)
    
    **Funcionalidades:**
    
    - Coordina flujo para usuarios normales
    - Aplica restricciones de seguridad por RUT/Transportista
    - Selecciona agente especializado según la consulta

    ??? info "Flujo de Selección de Agente"
        ```javascript
        const selectAgent = (userQuery) => {
          if (containsKeywords(query, ['guia', 'documento', 'pdf'])) {
            return documentAgent;
          } else if (containsKeywords(query, ['combustible', 'diesel'])) {
            return fuelAgent;
          } else if (containsKeywords(query, ['viaje', 'operacion'])) {
            return operationAgent;
          } else if (containsKeywords(query, ['vehiculo', 'camion'])) {
            return vehicleAgent;
          } else if (containsKeywords(query, ['transportista', 'empresa'])) {
            return transporterAgent;
          }
          return userAgent; // Default
        };
        ```

    #### Admin Orchestrator (`adminOrchestrator.js`)
    
    **Funcionalidades:**
    
    - Maneja consultas administrativas
    - Acceso sin restricciones a toda la base de datos
    - Proporciona análisis y reportes avanzados

=== "Agentes de IA"
    ### 4. Agentes de IA

    #### Agentes de Usuario

    | Agente | Archivo | Descripción |
    |--------|---------|-------------|
    | **User Agent** | `userAgent.js` | Agente conversacional para usuarios finales con restricciones de seguridad |
    | **Admin Agent** | `adminAgent.js` | Agente con privilegios administrativos completos |

    #### Agentes de Base de Datos (Factory Pattern)

    | Agente | Archivo | Especialización |
    |--------|---------|-----------------|
    | **Document Agent** | `documentAgent.js` | Guías, documentos y trazabilidad |
    | **Fuel Agent** | `fuelAgent.js` | Combustible y consumos |
    | **Operation Agent** | `operationAgent.js` | Operaciones y viajes |
    | **Vehicle Agent** | `vehicleAgent.js` | Vehículos y flotas |
    | **Transporter Agent** | `transporterAgent.js` | Información de transportistas |

    ??? info "Factory Pattern"
        ```javascript
        class AgentFactory {
          static createAgent(type, context) {
            switch(type) {
              case 'document': return new DocumentAgent(context);
              case 'fuel': return new FuelAgent(context);
              case 'operation': return new OperationAgent(context);
              case 'vehicle': return new VehicleAgent(context);
              case 'transporter': return new TransporterAgent(context);
              default: throw new Error('Unknown agent type');
            }
          }
        }
        ```

=== "MCP Protocol"
    ### 5. Model Context Protocol (MCP)

    #### MCP Client (`dbClient.js`)

    Cliente que comunica con el servidor MCP para ejecutar operaciones de base de datos.

    **Métodos principales:**

    | Método | Descripción |
    |--------|-------------|
    | `executeSQL(query)` | Ejecuta consultas SQL |
    | `getUserByPhone(phone)` | Busca usuario por teléfono |
    | `getUserByConversationId(conversationId)` | Busca usuario por ID de conversación |
    | `updateConversationId(rut, conversationId)` | Actualiza vinculación |
    | `listTables()` | Lista tablas disponibles |
    | `getTableSchema(tableName)` | Obtiene esquema de tabla |

    ??? info "Ejemplo de Uso"
        ```javascript
        const mcpClient = require('./dbClient');
        
        // Ejecutar query SQL
        const result = await mcpClient.executeSQL(
          'SELECT * FROM Guias WHERE RutTransportista = ?',
          [userRut]
        );
        
        // Obtener usuario
        const user = await mcpClient.getUserByPhone('+56912345678');
        ```

    #### MCP Server (`dbServer.js`)

    Servidor que abstrae el acceso a SQL Server mediante el protocolo MCP.

    ??? warning "Seguridad MCP"
        - Conexión encriptada
        - Validación de queries
        - Rate limiting
        - Auditoría de consultas

=== "Utilidades"
    ### 6. Utilidades

    | Módulo | Archivo | Funcionalidad |
    |--------|---------|---------------|
    | **Conversation Memory** | `conversationMemory.js` | Mantiene contexto y estado de conversaciones en memoria |
    | **Message Formatter** | `messageFormatter.js` | Formatea respuestas para WhatsApp |
    | **Toon Formatter** | `toonFormatter.js` | Convierte resultados SQL a formato estructurado legible |
    | **Interactive Messages** | `interactiveMessages.js` | Genera botones y menús interactivos de WhatsApp |

    ??? info "Ejemplo Message Formatter"
        ```javascript
        const formatter = require('./messageFormatter');
        
        // Formatear respuesta simple
        const message = formatter.formatSimpleMessage(
          'Tu consulta ha sido procesada'
        );
        
        // Formatear tabla
        const table = formatter.formatTable(
          ['Guia', 'Estado', 'Fecha'],
          [
            ['12345', 'Confirmada', '2024-01-15'],
            ['12346', 'Pendiente', '2024-01-16']
          ]
        );
        ```

## Variables de Entorno

??? info "Configuración Completa - .env"
    ```
    ### ============================================
    ### SERVER CONFIGURATION
    ### ============================================
    PORT=3000
    
    ### ============================================
    ### WEBHOOK VERIFICATION
    ### ============================================
    VERIFY_TOKEN=your_verify_token
    
    ### ============================================
    ### KAPSO WHATSAPP API
    ### ============================================
    KAPSO_API_KEY=your_kapso_api_key
    KAPSO_API_URL=https://api.kapso.com
    
    ### ============================================
    ### OPENAI CONFIGURATION
    ### ============================================
    OPENAI_API_KEY=your_openai_api_key
    
    ### ============================================
    ### DATABASE CONNECTION (MCP SERVER)
    ### ============================================
    DB_SERVER=your_server.database.windows.net
    DB_DATABASE=TracminDB
    DB_USER=your_username
    DB_PASSWORD=your_password
    DB_ENCRYPT=true
    
    ```
    
    | Variable | Descripción | Requerido |
    |----------|-------------|-----------|
    | `PORT` | Puerto del servidor webhook | ✅ Sí |
    | `VERIFY_TOKEN` | Token de verificación del webhook de Meta | ✅ Sí |
    | `KAPSO_API_KEY` | API Key de Kapso WhatsApp Cloud | ✅ Sí |
    | `KAPSO_API_URL` | URL base de la API de Kapso | ✅ Sí |
    | `OPENAI_API_KEY` | API Key de OpenAI para agentes IA | ✅ Sí |
    | `DB_SERVER` | Servidor SQL Server (MCP) | ✅ Sí |
    | `DB_DATABASE` | Nombre de la base de datos | ✅ Sí |
    | `DB_USER` | Usuario de la base de datos | ✅ Sí |
    | `DB_PASSWORD` | Contraseña de la base de datos | ✅ Sí |
    | `DB_ENCRYPT` | Cifrado de conexión a BD | ✅ Sí |

## Referencias

??? tip "Documentación Relacionada"
    - [Flujo de Mensajes](./flujo-mensajes.md) - Procesamiento detallado de mensajes
    - [Sistema de Agentes](./sistema-agentes.md) - Arquitectura de agentes IA
    - [Seguridad y Validación](./seguridad.md) - Guardrails y validaciones
    - [MCP y Base de Datos](./mcp-database.md) - Protocolo de contexto de modelo

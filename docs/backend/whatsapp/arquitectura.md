# Arquitectura del Webhook WhatsApp

## Descripción General

El webhook de WhatsApp es un sistema de mensajería inteligente basado en agentes de IA que permite a los usuarios interactuar con la plataforma Tracmin a través de WhatsApp. El sistema implementa arquitectura de agentes orquestados con acceso a base de datos mediante Model Context Protocol (MCP).

## Stack Tecnológico

```mermaid
graph LR
    A[Express.js] --> B[OpenAI Agents]
    B --> C[MCP Protocol]
    C --> D[SQL Server]
    A --> E[Kapso WhatsApp API]
    B --> F[Guardrails]
    B --> G[Toon Formatter]
```

### Tecnologías Principales

- **Runtime**: Node.js
- **Framework Web**: Express.js 4.21.2
- **API WhatsApp**: @kapso/whatsapp-cloud-api 0.1.1
- **IA/Agentes**: @openai/agents latest
- **Seguridad**: @openai/guardrails latest
- **Database Protocol**: @modelcontextprotocol/sdk 1.0.4
- **Database**: mssql 11.0.1 (SQL Server)
- **Formato de respuestas**: @toon-format/toon 0.9.0
- **HTTP Client**: node-fetch 3.3.2
- **Config**: dotenv 16.4.7

## Arquitectura General del Sistema

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

## Flujo de Procesamiento de Mensajes

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

### 1. Express Webhook (`app.js`, `webhook.js`)

Punto de entrada del sistema que recibe eventos de WhatsApp.

**Endpoints:**

- `GET /` - Verificación de webhook (handshake con Kapso)
- `POST /` - Recepción de mensajes y eventos

### 2. Capa de Seguridad

#### Content Filter (`contentFilter.js`)

Bloquea solicitudes relacionadas con trabajo o código malicioso.

#### Guardrails (`guardrails.js`)

Valida entrada/salida usando OpenAI Moderation API y filtros personalizados.

#### RUT Validator (`rutValidator.js`)

Valida identidad chilena (RUT) y vinculación de cuentas.

#### Auth Service (`authService.js`)

Gestiona autenticación y permisos de usuarios.

### 3. Orquestadores

#### User Orchestrator (`userOrchestrator.js`)

- Coordina flujo para usuarios normales
- Aplica restricciones de seguridad por RUT/Transportista
- Selecciona agente especializado según la consulta

#### Admin Orchestrator (`adminOrchestrator.js`)

- Maneja consultas administrativas
- Acceso sin restricciones a toda la base de datos
- Proporciona análisis y reportes avanzados

### 4. Agentes de IA

#### Agentes de Usuario

**User Agent** (`userAgent.js`): Agente conversacional para usuarios finales con restricciones de seguridad.

**Admin Agent** (`adminAgent.js`): Agente con privilegios administrativos completos.

#### Agentes de Base de Datos (Factory Pattern)

**Document Agent** (`documentAgent.js`): Especializado en guías, documentos y trazabilidad.

**Fuel Agent** (`fuelAgent.js`): Gestiona consultas de combustible y consumos.

**Operation Agent** (`operationAgent.js`): Maneja operaciones y viajes.

**Vehicle Agent** (`vehicleAgent.js`): Consultas sobre vehículos y flotas.

**Transporter Agent** (`transporterAgent.js`): Información de transportistas.

### 5. Model Context Protocol (MCP)

#### MCP Client (`dbClient.js`)

Cliente que comunica con el servidor MCP para ejecutar operaciones de base de datos.

**Métodos principales:**

- `executeSQL(query)` - Ejecuta consultas SQL
- `getUserByPhone(phone)` - Busca usuario por teléfono
- `getUserByConversationId(conversationId)` - Busca usuario por ID de conversación
- `updateConversationId(rut, conversationId)` - Actualiza vinculación
- `listTables()` - Lista tablas disponibles
- `getTableSchema(tableName)` - Obtiene esquema de tabla

#### MCP Server (`dbServer.js`)

Servidor que abstrae el acceso a SQL Server mediante el protocolo MCP.

### 6. Utilidades

#### Conversation Memory (`conversationMemory.js`)

Mantiene contexto y estado de conversaciones en memoria.

#### Message Formatter (`messageFormatter.js`)

Formatea respuestas para WhatsApp.

#### Toon Formatter (`toonFormatter.js`)

Convierte resultados SQL a formato estructurado legible.

#### Interactive Messages (`interactiveMessages.js`)

Genera botones y menús interactivos de WhatsApp.

## Variables de Entorno

```env
# Server
PORT=3000

# Webhook Verification
VERIFY_TOKEN=your_verify_token

# Kapso WhatsApp API
KAPSO_API_KEY=your_kapso_api_key
KAPSO_API_URL=https://api.kapso.com

# OpenAI
OPENAI_API_KEY=your_openai_api_key

# Database Connection (para MCP Server)
DB_SERVER=your_server.database.windows.net
DB_DATABASE=TracminDB
DB_USER=your_username
DB_PASSWORD=your_password
DB_ENCRYPT=true
```

## Próximos Pasos

- [Flujo de Mensajes](./flujo-mensajes.md)
- [Sistema de Agentes](./sistema-agentes.md)
- [Seguridad y Validación](./seguridad.md)
- [MCP y Base de Datos](./mcp-database.md)

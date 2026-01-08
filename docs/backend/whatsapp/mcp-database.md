# Model Context Protocol (MCP) y Base de Datos

## ¿Qué es MCP?

El **Model Context Protocol** es un protocolo estándar para la comunicación entre agentes de IA y fuentes de datos. Permite que los modelos de lenguaje accedan a información estructurada de manera segura y eficiente.

```mermaid
graph LR
    A[Agent IA] -->|Solicitud MCP| B[MCP Client]
    B -->|Protocolo stdio| C[MCP Server]
    C -->|SQL Query| D[(Database)]
    D -->|Results| C
    C -->|JSON Response| B
    B -->|Datos formateados| A
    
    style B fill:#FFD700
    style C fill:#87CEEB
    style D fill:#CC2927
```

## Arquitectura MCP en el Webhook

```mermaid
graph TB
    subgraph "Agentes IA"
        UA[User Agent]
        AA[Admin Agent]
        DA[Database Agents]
    end
    
    subgraph "MCP Client Layer"
        MC[DatabaseMCPClient<br/>Singleton]
        TRANS[Transport Layer<br/>stdio]
    end
    
    subgraph "MCP Server Process"
        MS[MCP Server<br/>dbServer.js]
        TOOLS[Tools Registry]
        EXEC[SQL Executor]
    end
    
    subgraph "Database Layer"
        POOL[Connection Pool<br/>mssql]
        DB[(SQL Server<br/>TracminDB)]
    end
    
    UA --> MC
    AA --> MC
    DA --> MC
    
    MC --> TRANS
    TRANS -->|stdio IPC| MS
    MS --> TOOLS
    TOOLS --> EXEC
    EXEC --> POOL
    POOL --> DB
    
    DB -->|Results| POOL
    POOL -->|Data| EXEC
    EXEC -->|JSON| TOOLS
    TOOLS -->|Response| MS
    MS -->|stdio| TRANS
    TRANS -->|Parsed| MC
    MC -->|Formatted| UA
    
    style MC fill:#4A90E2
    style MS fill:#E24A90
    style TOOLS fill:#90E24A
```

## MCP Client

### Singleton Pattern

El cliente MCP usa patrón Singleton para reutilizar la conexión:

```javascript
class DatabaseMCPClient {
  constructor() {
    this.client = null;
    this.transport = null;
    this.isConnecting = false;
    this.connectionPromise = null;
  }

  async connect() {
    // Si ya existe conexión, retornarla
    if (this.client) {
      return this.client;
    }

    // Si está conectando, esperar la promesa existente
    if (this.isConnecting) {
      return this.connectionPromise;
    }

    // Iniciar nueva conexión
    this.isConnecting = true;
    this.connectionPromise = this._connect();

    try {
      await this.connectionPromise;
      return this.client;
    } finally {
      this.isConnecting = false;
    }
  }
}

// Exportar instancia única
let clientInstance = null;

function getDBClient() {
  if (!clientInstance) {
    clientInstance = new DatabaseMCPClient();
  }
  return clientInstance;
}
```

### Flujo de Conexión

```mermaid
sequenceDiagram
    participant A as Agent
    participant C as MCP Client
    participant T as Transport
    participant S as MCP Server
    participant DB as Database
    
    A->>C: getDBClient()
    
    alt Primera conexión
        C->>C: Crear nueva instancia
        C->>T: new StdioClientTransport()
        T->>S: Spawn proceso node
        S->>DB: Crear connection pool
        DB-->>S: Pool ready
        S-->>T: Server ready
        T-->>C: Connected
        C->>C: Listar herramientas
        C-->>A: Client ready
    else Conexión existente
        C-->>A: Retornar client existente
    end
    
    A->>C: executeSQL(query)
    C->>S: call_tool('execute_sql')
    S->>DB: Execute query
    DB-->>S: Results
    S-->>C: JSON response
    C-->>A: Parsed data
```

### Métodos del Cliente

```javascript
class DatabaseMCPClient {
  // Ejecutar SQL personalizado
  async executeSQL(query, params = {}) {
    await this.connect();
    return await this.client.callTool('execute_sql', {
      query,
      parameters: params
    });
  }

  // Listar todas las tablas
  async listTables() {
    await this.connect();
    return await this.client.callTool('list_tables', {});
  }

  // Obtener esquema de una tabla
  async getTableSchema(tableName) {
    await this.connect();
    return await this.client.callTool('get_table_schema', {
      table_name: tableName
    });
  }

  // Buscar usuario por teléfono
  async getUserByPhone(phoneNumber) {
    const query = `
      SELECT 
        Rut,
        Nombre,
        Email,
        CodigoTransportista,
        IdConversacion,
        Admin,
        Activo
      FROM Usuarios
      WHERE Telefono = @phone
      AND Activo = 1
    `;
    
    const result = await this.executeSQL(query, { phone: phoneNumber });
    return result.length > 0 ? result[0] : null;
  }

  // Actualizar conversationId
  async updateConversationId(rut, conversationId) {
    const query = `
      UPDATE Usuarios
      SET IdConversacion = @conversationId,
          FechaActualizacion = GETDATE()
      WHERE Rut = @rut
    `;
    
    return await this.executeSQL(query, { rut, conversationId });
  }
}
```

## MCP Server

### Registro de Tools

```javascript
const server = new Server(
  {
    name: 'tracmin-database-server',
    version: '1.0.0',
  },
  {
    capabilities: {
      tools: {},
    },
  }
);

// Registrar herramientas disponibles
server.setRequestHandler(ListToolsRequestSchema, async () => {
  return {
    tools: [
      {
        name: 'execute_sql',
        description: 'Execute a SQL query and return results',
        inputSchema: {
          type: 'object',
          properties: {
            query: {
              type: 'string',
              description: 'SQL query to execute'
            },
            parameters: {
              type: 'object',
              description: 'Query parameters'
            }
          },
          required: ['query']
        }
      },
      {
        name: 'list_tables',
        description: 'List all available tables in the database',
        inputSchema: {
          type: 'object',
          properties: {}
        }
      },
      {
        name: 'get_table_schema',
        description: 'Get the schema of a specific table',
        inputSchema: {
          type: 'object',
          properties: {
            table_name: {
              type: 'string',
              description: 'Name of the table'
            }
          },
          required: ['table_name']
        }
      }
    ]
  };
});
```

### Handler de Herramientas

```mermaid
flowchart TD
    Start([Call Tool Request]) --> Parse[Parse tool name y arguments]
    
    Parse --> CheckTool{¿Qué tool?}
    
    CheckTool -->|execute_sql| ValidateSQL[Validar SQL]
    CheckTool -->|list_tables| ListTables[Listar tablas]
    CheckTool -->|get_table_schema| GetSchema[Obtener esquema]
    
    ValidateSQL --> CheckParams{¿Tiene<br/>parámetros?}
    CheckParams -->|Sí| PrepareParams[Preparar params]
    CheckParams -->|No| PrepareQuery[Preparar query]
    PrepareParams --> PrepareQuery
    
    PrepareQuery --> ExecuteSQL[Ejecutar en SQL Server]
    ListTables --> ExecuteSQL
    GetSchema --> ExecuteSQL
    
    ExecuteSQL --> CheckError{¿Error?}
    
    CheckError -->|Sí| LogError[Log error]
    LogError --> ReturnError[Retornar error]
    ReturnError --> End([End])
    
    CheckError -->|No| FormatResults[Formatear resultados<br/>a JSON]
    FormatResults --> ReturnSuccess[Retornar success]
    ReturnSuccess --> End
    
    style ExecuteSQL fill:#87CEEB
    style LogError fill:#FF6B6B
    style FormatResults fill:#90EE90
```

### Implementación de execute_sql

```javascript
server.setRequestHandler(CallToolRequestSchema, async (request) => {
  const { name, arguments: args } = request.params;

  if (name === 'execute_sql') {
    try {
      const { query, parameters = {} } = args;
      
      console.log('[MCP Server] Ejecutando SQL:', query);
      console.log('[MCP Server] Parámetros:', parameters);

      // Conectar al pool
      await ensureConnection();

      // Crear request
      const sqlRequest = pool.request();

      // Agregar parámetros
      for (const [key, value] of Object.entries(parameters)) {
        sqlRequest.input(key, value);
      }

      // Ejecutar query
      const result = await sqlRequest.query(query);

      console.log(`[MCP Server] ✅ Query exitosa: ${result.recordset.length} filas`);

      return {
        content: [
          {
            type: 'text',
            text: JSON.stringify(result.recordset, null, 2)
          }
        ]
      };

    } catch (error) {
      console.error('[MCP Server] ❌ Error ejecutando SQL:', error);
      
      return {
        content: [
          {
            type: 'text',
            text: JSON.stringify({
              error: error.message,
              code: error.code
            })
          }
        ],
        isError: true
      };
    }
  }

  if (name === 'list_tables') {
    const query = `
      SELECT 
        TABLE_SCHEMA,
        TABLE_NAME,
        TABLE_TYPE
      FROM INFORMATION_SCHEMA.TABLES
      WHERE TABLE_TYPE = 'BASE TABLE'
      ORDER BY TABLE_SCHEMA, TABLE_NAME
    `;
    
    // Ejecutar y retornar...
  }

  if (name === 'get_table_schema') {
    const query = `
      SELECT 
        COLUMN_NAME,
        DATA_TYPE,
        IS_NULLABLE,
        CHARACTER_MAXIMUM_LENGTH,
        COLUMN_DEFAULT
      FROM INFORMATION_SCHEMA.COLUMNS
      WHERE TABLE_NAME = @tableName
      ORDER BY ORDINAL_POSITION
    `;
    
    // Ejecutar y retornar...
  }
});
```

## Connection Pooling

### Configuración del Pool

```javascript
const config = {
  server: process.env.DB_SERVER,
  database: process.env.DB_DATABASE,
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  options: {
    encrypt: true,
    trustServerCertificate: false,
    enableArithAbort: true,
    connectTimeout: 30000,
    requestTimeout: 30000
  },
  pool: {
    max: 10,
    min: 2,
    idleTimeoutMillis: 30000,
    acquireTimeoutMillis: 30000
  }
};

let pool = null;

async function ensureConnection() {
  if (!pool || !pool.connected) {
    console.log('[MCP Server] Conectando a SQL Server...');
    pool = await sql.connect(config);
    console.log('[MCP Server] ✅ Conectado a SQL Server');
  }
  return pool;
}
```

### Lifecycle del Pool

```mermaid
stateDiagram-v2
    [*] --> Disconnected
    
    Disconnected --> Connecting: ensureConnection()
    
    Connecting --> Connected: Success
    Connecting --> Error: Connection failed
    
    Error --> Disconnected: Retry
    
    Connected --> Active: Execute query
    Active --> Connected: Query complete
    
    Connected --> Idle: No activity
    Idle --> Connected: New request
    Idle --> Disconnected: Timeout
    
    Connected --> Error: Network issue
    
    Error --> [*]: Fatal error
    Disconnected --> [*]: Shutdown
```

## Schema del Proyecto

### Archivo dbSchema.json

El esquema de base de datos está documentado en formato JSON para que los agentes lo entiendan:

```json
{
  "database": "TracminDB",
  "tables": {
    "Usuarios": {
      "description": "Usuarios del sistema con información de autenticación",
      "columns": {
        "Rut": "VARCHAR(12) - RUT del usuario (PK)",
        "Nombre": "VARCHAR(200) - Nombre completo",
        "Email": "VARCHAR(200) - Correo electrónico",
        "Telefono": "VARCHAR(20) - Número telefónico",
        "IdConversacion": "VARCHAR(100) - ID de conversación WhatsApp",
        "CodigoTransportista": "VARCHAR(20) - Código del transportista",
        "Admin": "BIT - Es administrador",
        "Activo": "BIT - Usuario activo"
      },
      "indexes": ["Rut", "IdConversacion", "Telefono"]
    },
    "GuiasDespacho": {
      "description": "Guías de despacho de mercancías",
      "columns": {
        "IdGuia": "INT IDENTITY - ID único (PK)",
        "NumeroFolio": "VARCHAR(50) - Número de folio",
        "RutTransportista": "VARCHAR(12) - RUT del transportista",
        "FechaEmision": "DATETIME - Fecha de emisión",
        "Estado": "VARCHAR(50) - Estado actual",
        "TipoDocumento": "VARCHAR(50) - Tipo de documento"
      },
      "security": {
        "user_filter": "RutTransportista = @userRut",
        "admin_filter": "none"
      }
    },
    "Operaciones": {
      "description": "Operaciones de transporte y viajes",
      "columns": {
        "IdOperacion": "INT IDENTITY - ID único (PK)",
        "RutTransportista": "VARCHAR(12) - RUT del transportista",
        "Patente": "VARCHAR(10) - Patente del vehículo",
        "FechaOperacion": "DATE - Fecha de operación",
        "Estado": "VARCHAR(50) - Estado de la operación"
      },
      "security": {
        "user_filter": "RutTransportista = @userRut",
        "admin_filter": "none"
      }
    }
  }
}
```

## Integración con Agentes

### Uso desde un Agente

```javascript
async function queryUserDocuments(userRut, startDate, endDate) {
  const dbClient = getDBClient();
  
  const query = `
    SELECT 
      NumeroFolio,
      FechaEmision,
      Estado,
      TipoDocumento
    FROM GuiasDespacho
    WHERE RutTransportista = @rut
    AND FechaEmision BETWEEN @startDate AND @endDate
    ORDER BY FechaEmision DESC
  `;
  
  const results = await dbClient.executeSQL(query, {
    rut: userRut,
    startDate,
    endDate
  });
  
  return results;
}
```

### Formato de Resultados con Toon

```javascript
const { formatToon, wrapToonForLLM } = require('../utils/toonFormatter');

function formatDatabaseResult(results, queryContext) {
  // Convertir a formato Toon
  const toonData = formatToon(results);
  
  // Envolver para LLM
  const wrappedToon = wrapToonForLLM(toonData, queryContext);
  
  return wrappedToon;
}
```

## Manejo de Errores

### Tipos de Errores

```mermaid
graph TD
    E[Error en MCP] --> T{Tipo de error}
    
    T -->|Connection| C[Error de conexión]
    T -->|Query| Q[Error de SQL]
    T -->|Timeout| TO[Timeout]
    T -->|Permission| P[Error de permisos]
    
    C --> R1[Reintentar conexión]
    R1 --> L1[Log error]
    
    Q --> A1[Analizar query]
    A1 --> L2[Log query + error]
    
    TO --> I1[Incrementar timeout]
    I1 --> L3[Log warning]
    
    P --> N1[Notificar admin]
    N1 --> L4[Log critical]
    
    L1 --> U[Mensaje usuario]
    L2 --> U
    L3 --> U
    L4 --> U
    
    style C fill:#FFA500
    style Q fill:#FF6B6B
    style TO fill:#FFD700
    style P fill:#FF0000
```

### Reintentos y Fallback

```javascript
async function executeWithRetry(fn, maxRetries = 3) {
  let lastError;
  
  for (let i = 0; i < maxRetries; i++) {
    try {
      return await fn();
    } catch (error) {
      lastError = error;
      console.log(`[MCP] Intento ${i + 1} falló:`, error.message);
      
      // Esperar antes de reintentar
      await new Promise(resolve => setTimeout(resolve, 1000 * (i + 1)));
    }
  }
  
  throw new Error(`Falló después de ${maxRetries} intentos: ${lastError.message}`);
}

// Uso
const results = await executeWithRetry(() => 
  dbClient.executeSQL(query, params)
);
```

## Performance y Optimización

### Caching de Esquemas

```javascript
class DatabaseMCPClient {
  constructor() {
    this.schemaCache = new Map();
  }

  async getTableSchema(tableName) {
    // Check cache
    if (this.schemaCache.has(tableName)) {
      return this.schemaCache.get(tableName);
    }

    // Fetch from DB
    const schema = await this._fetchSchema(tableName);
    
    // Cache por 1 hora
    this.schemaCache.set(tableName, schema);
    setTimeout(() => {
      this.schemaCache.delete(tableName);
    }, 3600000);

    return schema;
  }
}
```

### Query Optimization

- Usar índices apropiados
- Limitar resultados con TOP o LIMIT
- Evitar SELECT * en producción
- Usar parámetros para prevenir SQL injection
- Aplicar filtros de seguridad siempre

## Monitoreo

### Logs de MCP

```javascript
function logMCPOperation(operation, details) {
  console.log(JSON.stringify({
    timestamp: new Date().toISOString(),
    component: 'MCP',
    operation,
    ...details
  }));
}

// Uso
logMCPOperation('execute_sql', {
  query: query.substring(0, 100),
  params: Object.keys(params),
  duration: endTime - startTime,
  rows: result.recordset.length
});
```

## Próximos Pasos

- [Configuración y Despliegue](./configuracion.md)
- [Troubleshooting](./troubleshooting.md)
- [Best Practices](./best-practices.md)

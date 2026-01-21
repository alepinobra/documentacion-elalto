# 📡 API Endpoints - Documentación Completa

## 📋 Descripción General

Documentación detallada de todos los endpoints REST disponibles en la API de Control de Acceso.

**Base URL**: `https://AzureDirection/appaccesocontrol`

## 🔍 Índice de Endpoints

```mermaid
graph LR
    API[API Control Acceso] --> HEALTH[Health]
    API --> AUTH[Autenticación]
    API --> PORTONES[Portones]
    API --> MQTT[MQTT/IoT]
    API --> INVITADOS[Invitados]
    
    AUTH --> LOGIN[POST /login]
    AUTH --> GUEST[POST /invitados/login]
    
    PORTONES --> GET_PORT[POST /portones]
    
    MQTT --> MQTT_TEST[POST /mqtt_test]
    
    INVITADOS --> CREATE[POST /invitados/crear]
    INVITADOS --> LIST[POST /invitados/listar]
    INVITADOS --> UPDATE[PUT /invitados/actualizar]
    
    style AUTH fill:#FF6B6B
    style MQTT fill:#660066
    style INVITADOS fill:#90EE90
```

## 🏥 Health Check

### GET /health

Verifica el estado del servidor.

**Autenticación**: No requerida

**Request**:
```http
GET /appaccesocontrol/health HTTP/1.1
Host: AzureDirection
```

**Response** (200 OK):
```json
{
  "status": "healthy",
  "version": "3.5.3"
}
```

---

## 🔐 Autenticación

### POST /login

Autenticación de usuario normal con RUT/Email y contraseña.

**Autenticación**: No requerida

**Request**:
```http
POST /appaccesocontrol/login HTTP/1.1
Content-Type: application/json

{
  "rut_or_mail": "12345678-9",
  "password": "micontraseña"
}
```

**Request Body**:

| Campo | Tipo | Requerido | Descripción |
|-------|------|-----------|-------------|
| `rut_or_mail` | string | ✅ | RUT (12345678-9) o Email |
| `password` | string | ✅ | Contraseña |

**Response** (200 OK):
```json
{
  "id": 123,
  "nombre": "Juan",
  "apellido": "Pérez",
  "mail": "juan@example.com",
  "rut": "12345678-9",
  "admin": true,
  "invitar": true,
  "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

**Response Body**:


| Campo | Tipo | Descripción |
|-------|------|-------------|
| `id` | number | ID del usuario |
| `nombre` | string | Nombre |
| `apellido` | string | Apellido |
| `mail` | string | Email |
| `rut` | string | RUT |
| `admin` | boolean | Es administrador |
| `invitar` | boolean | Puede crear invitados |
| `access_token` | string | JWT token (válido 24h) |

**Errores**:

- **401 Unauthorized**: Credenciales incorrectas
```json
{
  "detail": "Credenciales inválidas"
}
```

**Ejemplo con curl**:
```bash
curl -X POST \
  https://AzureDirection/appaccesocontrol/login \
  -H 'Content-Type: application/json' \
  -d '{
    "rut_or_mail": "12345678-9",
    "password": "micontraseña"
  }'
```

---

### POST /invitados/login

Autenticación de invitado con código temporal de 4 dígitos.

**Autenticación**: No requerida

**Request**:
```http
POST /appaccesocontrol/invitados/login HTTP/1.1
Content-Type: application/json

{
  "CodigoAcceso": "1234"
}
```

**Request Body**:

| Campo | Tipo | Requerido | Descripción |
|-------|------|-----------|-------------|
| `CodigoAcceso` | string | ✅ | Código de 4 dígitos |

**Response** (200 OK):
```json
{
  "IdInvitante": 123,
  "Nombre": "Pedro",
  "Apellido": "González",
  "Mail": "pedro@empresa.com",
  "FechaInicial": "2025-12-26T00:00:00Z",
  "FechaFinal": "2025-12-31T23:59:59Z",
  "Empresa": "Constructora ABC",
  "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

**Response Body**:

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `IdInvitante` | number | ID del usuario que invitó |
| `Nombre` | string | Nombre del invitado |
| `Apellido` | string | Apellido |
| `Mail` | string | Email |
| `FechaInicial` | string | Fecha inicio de acceso (ISO 8601) |
| `FechaFinal` | string | Fecha fin de acceso (ISO 8601) |
| `Empresa` | string | Empresa del invitado |
| `access_token` | string | JWT token |

**Errores**:

- **404 Not Found**: Código no encontrado
```json
{
  "detail": "Código de acceso no encontrado"
}
```

- **403 Forbidden**: Código ya usado
```json
{
  "detail": "Este código ya ha sido utilizado. Solicita una nueva invitación."
}
```

- **403 Forbidden**: Código expirado (más de 24h desde generación)
```json
{
  "detail": "El código ha expirado. Los códigos son válidos por 24 horas desde su generación."
}
```

- **403 Forbidden**: Fuera del rango de fechas
```json
{
  "detail": "El acceso aún no está disponible. Válido desde 2025-12-26T00:00:00Z"
}
```

**Ejemplo con curl**:
```bash
curl -X POST \
  https://AzureDirection/appaccesocontrol/invitados/login \
  -H 'Content-Type: application/json' \
  -d '{
    "CodigoAcceso": "1234"
  }'
```

---

## 🚪 Portones

### POST /portones

Obtiene la lista de portones disponibles para el usuario autenticado.

**Autenticación**: JWT Bearer Token requerido

**Request**:
```http
POST /appaccesocontrol/portones HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
Content-Type: application/json
```

**Headers**:

| Header | Tipo | Requerido | Descripción |
|--------|------|-----------|-------------|
| `Authorization` | string | ✅ | Bearer {token} |

**Response** (200 OK):
```json
{
  "portones": [
    {
      "IdPorton": 1,
      "NombrePorton": "Portón Principal",
      "IdShelly": "shellyplus1-a8032ab12345",
      "MensajeApertura": "Abriendo portón principal...",
      "LatitudPorton": -33.4569,
      "LongitudPorton": -70.6483,
      "Distancia": 300
    },
    {
      "IdPorton": 2,
      "NombrePorton": "Portón Estacionamiento",
      "IdShelly": "shellyplus1-a8032ab67890",
      "MensajeApertura": "Abriendo estacionamiento...",
      "LatitudPorton": -33.4580,
      "LongitudPorton": -70.6490,
      "Distancia": 300
    }
  ]
}
```

**Response Body**:

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `portones` | array | Array de portones |
| `portones[].IdPorton` | number | ID único del portón |
| `portones[].NombrePorton` | string | Nombre descriptivo |
| `portones[].IdShelly` | string | ID del dispositivo Shelly |
| `portones[].MensajeApertura` | string | Mensaje al abrir |
| `portones[].LatitudPorton` | number | Coordenada latitud |
| `portones[].LongitudPorton` | number | Coordenada longitud |
| `portones[].Distancia` | number | Radio en metros (300m) |

**Errores**:

- **401 Unauthorized**: Token inválido
```json
{
  "detail": "Could not validate credentials"
}
```

**Ejemplo con curl**:
```bash
curl -X POST \
  https://AzureDirection/appaccesocontrol/portones \
  -H 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...' \
  -H 'Content-Type: application/json'
```

---

## 🔌 MQTT / IoT

### POST /mqtt_test

Envía comandos MQTT a dispositivos Shelly para controlar portones.

**Autenticación**: API Key en header

**Request**:
```http
POST /appaccesocontrol/mqtt_test HTTP/1.1
Content-Type: application/json
key: tu-api-key-secreta

{
  "Lugar": "shellyplus1-a8032ab12345",
  "Comando": "open",
  "Topico": "romana_porton_rss"
}
```

**Headers**:

| Header | Tipo | Requerido | Descripción |
|--------|------|-----------|-------------|
| `key` | string | ✅ | API Key (config.USERS_KEY) |

**Request Body**:

| Campo | Tipo | Requerido | Descripción |
|-------|------|-----------|-------------|
| `Lugar` | string | ✅ | ID del dispositivo Shelly o nombre del lugar |
| `Comando` | string | ✅ | Comando: `open`, `close` |
| `Topico` | string | ✅ | Topic MQTT base (sin /rpc) |

**Comandos disponibles**:

- `open` - Activar relé (abrir portón)
- `close` - Desactivar relé (cerrar portón)

**Lugares válidos**:

- `Porton Estacionamiento Romana` → Switch ID 0
- `Porton Portería (Salida)` → Switch ID 0
- `Rodiluvio` → Switch ID 1
- `Barrera Romana` → Switch ID 1
- `Porton Agricola` → Switch ID 2

**Response** (200 OK):
```json
{
  "message": "ok",
  "lugar": "Porton Estacionamiento Romana",
  "comando": "open",
  "topic": "romana_porton_rss/rpc",
  "acciones_ejecutadas": [
    {
      "switch_id": 0,
      "on": true,
      "status": "enviado"
    }
  ]
}
```

**Response Body**:

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `message` | string | Estado: "ok" o "error" |
| `lugar` | string | Lugar recibido |
| `comando` | string | Comando ejecutado |
| `topic` | string | Topic MQTT usado |
| `acciones_ejecutadas` | array | Acciones enviadas |

**Errores**:

- **401 Unauthorized**: Key inválida
```json
{
  "message": "error",
  "error": "Key inválida"
}
```

- **400 Bad Request**: Lugar no encontrado
```json
{
  "message": "error",
  "error": "El porton Lugar Inexistente no fue encontrado:"
}
```

- **500 Internal Server Error**: Error MQTT
```json
{
  "message": "error",
  "error": "Error al enviar comandos MQTT: Connection refused"
}
```

**Payload MQTT generado**:
```json
{
  "id": 101,
  "src": "prueba-cli",
  "method": "Switch.Set",
  "params": {
    "id": 0,
    "on": true
  }
}
```

**Ejemplo con curl**:
```bash
curl -X POST \
  https://AzureDirection/appaccesocontrol/mqtt_test \
  -H 'Content-Type: application/json' \
  -H 'key: tu-api-key-secreta' \
  -d '{
    "Lugar": "Porton Estacionamiento Romana",
    "Comando": "open",
    "Topico": "romana_porton_rss"
  }'
```

---

## 👥 Invitados

### POST /invitados/crear

Crea un nuevo invitado y genera un código de acceso único de 4 dígitos.

**Autenticación**: JWT Bearer Token (solo administradores)

**Request**:
```http
POST /appaccesocontrol/invitados/crear HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
Content-Type: application/json

{
  "IdInvitante": 123,
  "Rut": "11111111-1",
  "Nombre": "Carlos",
  "Apellido": "Silva",
  "Telefono": "+56912345678",
  "Mail": "carlos@empresa.com",
  "FechaInicial": "2025-12-26T00:00:00Z",
  "FechaFinal": "2025-12-31T23:59:59Z",
  "Empresa": "Constructora ABC",
  "Motivo": "Visita de obra",
  "IdAcceso": 1
}
```

**Request Body**:

| Campo | Tipo | Requerido | Descripción |
|-------|------|-----------|-------------|
| `IdInvitante` | number | ✅ | ID del usuario que invita |
| `Rut` | string | ❌ | RUT del invitado |
| `Nombre` | string | ✅ | Nombre |
| `Apellido` | string | ✅ | Apellido |
| `Telefono` | string | ❌ | Teléfono (+56912345678) |
| `Mail` | string | ✅ | Email (debe ser válido) |
| `FechaInicial` | string | ✅ | Fecha inicio (ISO 8601) |
| `FechaFinal` | string | ✅ | Fecha fin (ISO 8601) |
| `Empresa` | string | ❌ | Empresa |
| `Motivo` | string | ❌ | Motivo de la visita |
| `IdAcceso` | number | ❌ | Tipo de acceso |

**Response** (201 Created):
```json
{
  "message": "Invitado creado y código enviado correctamente",
  "invitado": {
    "IdInvitante": 123,
    "Rut": "11111111-1",
    "Nombre": "Carlos",
    "Apellido": "Silva",
    "Telefono": "+56912345678",
    "Mail": "carlos@empresa.com",
    "CodigoAcceso": "5678",
    "FechaInicial": "2025-12-26T00:00:00Z",
    "FechaFinal": "2025-12-31T23:59:59Z",
    "Empresa": "Constructora ABC",
    "Motivo": "Visita de obra",
    "IdAcceso": 1
  }
}
```

**Response Body**:

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `message` | string | Mensaje de confirmación |
| `invitado` | object | Datos del invitado creado |
| `invitado.CodigoAcceso` | string | Código de 4 dígitos generado |

**Comportamiento**:

1. Si el email ya existe, actualiza el invitado existente con un nuevo código
2. Genera código único de 4 dígitos
3. Envía correo electrónico con el código al invitado
4. El código expira en 24 horas desde la generación

**Errores**:

- **401 Unauthorized**: Token inválido
- **403 Forbidden**: Usuario no es administrador
- **400 Bad Request**: Email inválido
```json
{
  "detail": "Email inválido"
}
```

- **500 Internal Server Error**: Error al crear
```json
{
  "detail": "Error al crear invitado: {mensaje}"
}
```

**Ejemplo con curl**:
```bash
curl -X POST \
  https://AzureDirection/appaccesocontrol/invitados/crear \
  -H 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...' \
  -H 'Content-Type: application/json' \
  -d '{
    "IdInvitante": 123,
    "Nombre": "Carlos",
    "Apellido": "Silva",
    "Mail": "carlos@empresa.com",
    "FechaInicial": "2025-12-26T00:00:00Z",
    "FechaFinal": "2025-12-31T23:59:59Z"
  }'
```

---

### POST /invitados/listar

Lista todos los invitados registrados.

**Autenticación**: JWT Bearer Token (solo administradores)

**Request**:
```http
POST /appaccesocontrol/invitados/listar HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
Content-Type: application/json
```

**Response** (200 OK):
```json
{
  "invitados": [
    {
      "IdInvitante": 123,
      "Rut": "11111111-1",
      "Nombre": "Carlos",
      "Apellido": "Silva",
      "Telefono": "+56912345678",
      "Mail": "carlos@empresa.com",
      "FechaInicial": "2025-12-26T00:00:00Z",
      "FechaFinal": "2025-12-31T23:59:59Z",
      "Empresa": "Constructora ABC",
      "Motivo": "Visita de obra",
      "IdAcceso": 1,
      "CodigoAcceso": "5678",
      "DiaAcceso": null
    },
    {
      "IdInvitante": 123,
      "Rut": "22222222-2",
      "Nombre": "Ana",
      "Apellido": "Martínez",
      "Mail": "ana@empresa.com",
      "FechaInicial": "2025-12-20T00:00:00Z",
      "FechaFinal": "2025-12-25T23:59:59Z",
      "CodigoAcceso": "1234",
      "DiaAcceso": "2025-12-22T10:30:00Z"
    }
  ]
}
```

**Errores**:

- **401 Unauthorized**: Token inválido
- **403 Forbidden**: Usuario no es administrador

---

## 📊 Códigos de Estado HTTP

| Código | Descripción | Uso |
|--------|-------------|-----|
| **200** | OK | Request exitoso |
| **201** | Created | Recurso creado exitosamente |
| **400** | Bad Request | Datos inválidos en el request |
| **401** | Unauthorized | Token JWT inválido o faltante |
| **403** | Forbidden | Sin permisos suficientes |
| **404** | Not Found | Recurso no encontrado |
| **500** | Internal Server Error | Error del servidor |

## 🔄 Flujo de Uso Típico

### Usuario Normal

```mermaid
sequenceDiagram
    participant App as Mobile App
    participant API as API Backend
    participant DB as Database
    participant MQTT as MQTT Broker
    participant Shelly as Dispositivo Shelly
    
    App->>API: POST /login
    API->>DB: Validate credentials
    DB-->>API: User data
    API-->>App: JWT token
    
    App->>API: POST /portones
    Note over App,API: Authorization: Bearer {token}
    API->>DB: Get user portones
    DB-->>API: Portones list
    API-->>App: Portones array
    
    App->>API: POST /mqtt_test
    Note over App,API: key: {api-key}
    API->>MQTT: Publish Switch.Set
    MQTT->>Shelly: Command
    Shelly-->>MQTT: ACK
    API-->>App: Success
```

### Administrador - Crear Invitado

```mermaid
sequenceDiagram
    participant Admin as Admin App
    participant API as API Backend
    participant DB as Database
    participant Mail as MS Graph API
    participant Guest as Invitado
    
    Admin->>API: POST /login
    API-->>Admin: JWT token (admin=true)
    
    Admin->>API: POST /invitados/crear
    API->>API: Generar código 4 dígitos
    API->>DB: Insert AMInvitados
    DB-->>API: ID created
    API->>Mail: Send email con código
    Mail->>Guest: Email con código
    API-->>Admin: Código generado
```

### Invitado

```mermaid
sequenceDiagram
    participant Guest as Invitado
    participant App as Mobile App
    participant API as API Backend
    participant DB as Database
    
    Guest->>App: Ingresa código 1234
    App->>API: POST /invitados/login
    API->>DB: Validate código
    
    alt Código válido
        DB-->>API: Invitado data
        API->>API: Check fechas
        API->>DB: Marcar CodigoUsado=true
        API-->>App: JWT token
        App->>API: POST /portones
        API-->>App: Portones permitidos
    else Código inválido
        API-->>App: 404 Not Found
    end
```

## 📚 Próximos Pasos

- [Arquitectura](./arquitectura.md)
- [Autenticación y Seguridad](./autenticacion.md)
- [Integración MQTT](./mqtt-dispositivos.md)
- [Base de Datos](./base-datos.md)
- [Sistema de Invitados](./invitados.md)

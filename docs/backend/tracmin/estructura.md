# Arquitectura Tracmin Backend - Documentación

## Arquitectura del Sistema

??? info "Tecnologías Core"
    - Framework Principal: FastAPI (Python)
    - Base de Datos: SQL Server
    - Base de Datos: SQL Server

??? tip "Seguridad"
    - Autenticación JWT
    - Control de roles y permisos
    - Encriptación de datos sensibles
    - Logs de auditoría

??? tip "Escalabilidad"
    - Arquitectura modular
    - Servicios independientes
    - Cache distribuido
    - Balanceo de carga
    - Monitorización
    - Logging centralizado
    - Métricas de rendimiento
    
```mermaid 
graph TD
    A[LogiPath API] --> B[SAP ERP]
    A --> C[Febos]
    A --> D[Microsoft Graph]
    A --> E[Azure Services]
    
    E --> F[WebPubSub]
    E --> G[Blob Storage]
    E --> H[Functions]
```
## Módulos Principales

### 1. Gestión de Viajes
- `Programación y seguimiento de viajes`
- `Control de estados y ubicaciones`
- `Gestión de conductores y vehículos`

### 2. Documentación Electrónica
- `Integración con SAP para guías de despacho`
- `Validación y control de documentos`
- `Seguimiento de estados documentales`

### 3. Comunicaciones
- `Notificaciones en tiempo real (WebPubSub)`
- `Correos automáticos (Microsoft Graph)`
- `Comunicación bidireccional con conductores`

### 4. Almacenamiento y Multimedia
- `Gestión de imágenes y documentos`
- `Almacenamiento en Azure Blob`
- `Respaldos y versionamiento`

```mermaid 
sequenceDiagram
    participant Cliente
    participant API
    participant SAP
    participant Febos
    participant Azure WebPubSub
    participant Azure Form Recognizer
    participant MS Graph
    participant Azure Blob

    Cliente->>API: Solicitud de Viaje
    
    par Proceso de Guía
        API->>SAP: Creación de Guía
        SAP-->>API: Confirmación
        API->>Febos: Generación Folio
        Febos-->>API: Folio Generado
    and Notificaciones
        API->>Azure WebPubSub: Broadcast Update
        Azure WebPubSub-->>Cliente: Actualización Tiempo Real
    end

    Cliente->>API: Upload Ticket/Imagen
    API->>Azure Blob: Almacenar Imagen
    Azure Blob-->>API: URL Imagen
    
    par Procesamiento Paralelo
        API->>Azure Form Recognizer: Análisis de Ticket
        Azure Form Recognizer-->>API: Datos Extraídos
    and Notificaciones Email
        API->>MS Graph: Envío Email Confirmación
        MS Graph-->>API: Email Enviado
    end

    API->>SAP: Actualización con Datos de Ticket
    SAP-->>API: Confirmación Final
    
    par Actualizaciones Finales
        API->>Azure WebPubSub: Notificar Completado
        Azure WebPubSub-->>Cliente: Update Final
    and Email Final
        API->>MS Graph: Email Resumen
        MS Graph-->>Cliente: Confirmación Email
    end
```

``` bash title="Dependencias" linenums="1"
fastapi = "^0.68.0"
sqlalchemy = "^1.4.23"
pyjwt = "^2.1.0"
azure-storage-blob = "^12.8.1"
azure-messaging-webpubsub = "^1.0.0"
```
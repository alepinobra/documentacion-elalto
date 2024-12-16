# API Subir Foto Pendiente - Documentación

## Descripción General
Endpoint para gestionar la carga de fotos pendientes de viajes anteriores, permitiendo asociar imágenes a viajes históricos y actualizar su información.

??? info "Notas Importantes"
    - Manejo de fotos pendientes de viajes pasados
    - Almacenamiento en Azure Blob Storage
    - Actualización de registros históricos
    - Manejo de metadatos del viaje
    - Proceso atómico de actualización

??? warning "Consideraciones Importantes"
    - La imagen debe estar en formato base64 válido
    - El viaje debe existir en la base de datos
    - Los campos se actualizan de forma selectiva
    - Se genera un nuevo nombre de blob único
    - Se mantiene la trazabilidad del viaje original
    - Configuración Requerida:
        ```bash title="Variables de entorno" linenums="1" 
        BLOB_CONN_STRING = "DefaultEndpointsProtocol=https;..."
        BLOB_CONTAINER_NAME = "container-name"
        ```

??? tip "Mejores Prácticas"
    - Validar el formato de la imagen
    - Implementar retry logic para Azure
    - Mantener logs detallados
    - Verificar permisos de actualización
    - Validar integridad de datos

## Endpoint Upload Photo Pending
`POST /api/logipath/upload_photo_pending`

=== "Flujo Explicado"
    ### 1. Validación y Preparación
    - `Verificación del token JWT`
    - `Decodificación de imagen base64`
    - `Logging de información crítica`
    ### 2. Proceso de Upload
    - `Conexión a Azure Blob Storage`
    - `Generación de nombre único`
    - `Subida de imagen con metadata`
    ### 3. Actualización de Viaje
    - `Búsqueda del viaje histórico`
    - `Extracción de metadata`
    - `Actualización de campos`
    - `Commit de cambios`

    ```json title="Estructura de API viajes" linenums="1" hl_lines="2-6"
    {
        "data": "string (base64)",
        "folio": "number",
        "IdProgramado": "number",
        "RutConductor": "string",
        "Id": "number"
    }
    ```
=== "Estados y Respuestas"
    ## Respuestas
    ```json title="Éxito" linenums="1" hl_lines="2-3"
    {
        "blob_name": "TKTxxxx-1234",
        "link": "https://storage.azure.com/...",
        "status_code": 200,
        "Cliente": "CLIENTE1",
        "HoraInicioJornada": "2024-01-01T12:00:00"
    }
    ``` 
    ```json title="Error" linenums="1" hl_lines="2"
    {
        "detail": "Mensaje específico del error",
        "status_code": 500
    }
    ``` 
=== "Flujo Diagrama"
    ```mermaid
    graph TD
        A[Cliente] --> B{Validar JWT}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D[Decodificar Base64]
        
        D --> E[Log Info]
        E --> F[Conectar Azure Blob]
        
        F --> G[Generar Nombre Blob]
        G --> H[Upload Imagen]
        
        H --> I[Buscar ActualTrip]
        I -->|No Encontrado| J[Error 404]
        I -->|Encontrado| K[Extraer Metadata]
        
        K --> L[Actualizar Campos]
        L --> M[Commit DB]
        M --> N[Return Success]
    ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Client
        participant API
        participant Azure Blob
        participant DB
        
        Client->>API: POST /upload_photo_pending
        API->>API: Validar JWT
        
        API->>API: Decodificar Base64
        API->>API: Log Info
        
        par Upload a Azure
            API->>Azure Blob: Upload Imagen
            Azure Blob-->>API: URL Imagen
        and Búsqueda en DB
            API->>DB: Buscar ActualTrip
            DB-->>API: Datos del Viaje
        end
        
        API->>API: Extraer Metadata
        API->>DB: Actualizar Campos
        DB-->>API: Commit Success
        
        API-->>Client: Response con Metadata
    ```
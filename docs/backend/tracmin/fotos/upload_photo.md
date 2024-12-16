# API Upload Photo

## Descripción General
Endpoint para subir fotos de tickets/comprobantes asociados a viajes, con almacenamiento en Azure Blob Storage y actualización de referencias en la base de datos.

??? info "Notas Importantes"
    - Requiere autenticación JWT
    - Manejo automático de zonas horarias
    - Generación aleatoria de nombres de blob
    - Almacenamiento en Azure Blob Storage
    - Actualización atómica en base de datos

??? warning "Consideraciones Importantes"
    - La imagen debe estar en formato base64
    - El timestamp debe estar en formato compatible
    - El tamaño máximo de la imagen puede estar limitado
    - La conexión a Azure debe estar configurada
    - El viaje debe existir en la base de datos

??? tip "Mejores Prácticas"
    - Validar el formato de la imagen antes del envío
    - Comprimir imágenes grandes
    - Implementar retry logic para fallos de Azure
    - Mantener logs detallados  
    - Validar tipos MIME

## Endpoint Upload Photo
`POST /api/logipath/upload_photo`


=== "Flujo Explicado"
    ### 1. Validación y Preparación
    - `Verificación del token JWT`
    - `Decodificación de imagen base64`
    - `Normalización de timestamp`
    ### 2. Almacenamiento en Azure
    - `Conexión a Blob Storage`
    - `Generación de nombre único`
    - `Upload de imagen`
    - `Configuración de content type`
    ### 3. Actualización en Base de Datos
    - `Búsqueda del viaje asociado`
    - `Actualización de referencia`
    - `Commit de cambios`
  
    ```json title="Estructura de upload photo" linenums="1" hl_lines="2-7"
    {
        "data": "string (base64)",
        "folio": "number",
        "cliente": "string",
        "IdProgramado": "number",
        "RutConductor": "string",
        "HoraInicioJornada": "datetime"
    }
    ```

=== "Estados y Respuestas"
    ## Respuestas Posibles
    ```json title="Éxito" linenums="1" hl_lines="2-3"
    {
        "blob_name": "TKTxxxx-1234",
        "link": "https://storage.azure.com/...",
        "status_code": 200
    }
    ```
    ```json title="No se encontró el viaje" linenums="1" hl_lines="2"
    {
        "message": "No se encontró el viaje"
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
        
        D --> E[Normalizar Timestamp]
        E --> F[Conectar Azure Blob]
        
        F --> G[Generar Nombre Blob]
        G --> H[Upload Imagen]
        
        H --> I[Buscar Viaje]
        I -->|No Encontrado| J[Return Not Found]
        I -->|Encontrado| K[Actualizar IdTicket]
        
        K --> L[Commit DB]
        L --> M[Return Success]
    ```
=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Client
        participant API
        participant Azure Blob
        participant DB
        
        Client->>API: POST /upload_photo
        API->>API: Validar JWT
        
        API->>API: Decodificar Base64
        
        API->>Azure Blob: Upload Imagen
        Azure Blob-->>API: URL Imagen
        
        API->>DB: Buscar Viaje
        
        alt Viaje Encontrado
            API->>DB: Actualizar IdTicket
            DB-->>API: Commit Success
            API-->>Client: Success + URL
        else Viaje No Encontrado
            API-->>Client: Not Found Error
        end
    ```
    

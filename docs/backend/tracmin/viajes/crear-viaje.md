# API CREAR VIAJES - Documentación

## Descripción General
Endpoint para la creación rápida de viajes programados con validación de duplicados.

??? info "Notas Importantes"
    - Requiere autenticación JWT con permisos de administrador
    - Límite de 3 viajes similares permitidos
    - Validación de duplicados basada en múltiples campos
    - Creación automática con estados iniciales predefinidos
    - Transacción atómica para garantizar consistencia  

??? warning "Consideraciones Importantes"
    - El límite de 3 viajes similares es una medida de seguridad
    - Los viajes se consideran similares si coinciden en todos los campos clave
    - La fecha programada debe estar en UTC
    - Se requieren permisos de administrador para usar este endpoint
    - La transacción es atómica para evitar estados inconsistentes (No existe estados intermedios)
    - Los estados iniciales son no modificables en la creación

## Endpoint Quick Trip
`POST /api/logipath/quick_trip`

=== "Flujo Explicado"
    ### 1. Validación de Seguridad
    - `Verificación de token JWT`
    - `Validación de permisos de administrador`

    ### 2. Verificación de Duplicados
    - `Búsqueda de viajes existentes con criterios múltiples:`
        - `RutConductor`
        - `Cliente`
        - `Patente`
        - `FechaProgramadaUtc`
        - `Programador`

    ### 3. Creación del Viaje
    - `Si hay menos de 3 viajes similares:`
        - `Creación de nuevo ScheduledTrip`
        - `Inicialización de estados`
        - `Commit en base de datos`

    ```json title="Estructura de API crear viajes" linenums="1" hl_lines="2-6"
    {
        "FechaProgramadaUtc": "datetime",
        "PlantaOrigen": "string",
        "Cliente": "string",
        "RutConductor": "string",
        "Patente": "string",
        "Programador": "string?"
    }
    ```

=== "Estados y Respuestas"
    ## Estados Iniciales
    - `Terminado: false`
    - `Cancelado: false`

    ## Respuestas
    ```json title="Éxito" linenums="1" hl_lines="2"
    {
        "message": "quick trip added successfully"
    }
    ``` 
    ```json title="Error Duplicado" linenums="1" hl_lines="2"
    {
        "message": "trip is already created",
        "Ids": [123, 124, 125]
    }
    ``` 
    ```json title="Error" linenums="1" hl_lines="2"
    {
        "detail": "Invalid authentication credentials",
        "status_code": 401
    }
    ```     

=== "Flujo Diagrama"
    ```mermaid
    graph TD
        A[Cliente] --> B{Validar JWT}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D[Verificar Admin]
        
        D -->|No Admin| E[Error 403]
        D -->|Admin| F[Buscar Viajes Similares]
        
        F --> G{Cantidad > 3?}
        G -->|Si| H[Retornar IDs Existentes]
        G -->|No| I[Crear Nuevo Viaje]
        
        I --> J[Inicializar Estados]
        J --> K[Commit DB]
        K --> L[Respuesta Success]
        
        H --> M[Respuesta con IDs]
    ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Client
        participant API
        participant DB
        
        Client->>API: POST /quick_trip
        API->>API: Validar JWT + Admin
        
        API->>DB: Buscar viajes similares
        DB-->>API: Lista de viajes
        
        alt Más de 3 viajes similares
            API-->>Client: IDs de viajes existentes
        else Menos de 3 viajes
            API->>DB: Crear nuevo ScheduledTrip
            DB-->>API: Commit successful
            API-->>Client: Confirmación de creación
        end
    ```
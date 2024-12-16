# API BUSCAR CANCELADO - Documentación

## Descripción General
Endpoint para verificar el estado de cancelación de una guía/viaje y obtener información relevante del viaje activo.

??? info "Notas Importantes"
    - Endpoint para verificar el estado de cancelación de una guía/viaje y obtener información relevante del viaje activo.
    - Normalización automática de timestamps (eliminación de microsegundos)
    - Búsqueda del viaje más reciente basado en múltiples criterios
    - Diferenciación entre viajes cancelados y activos
    - Retorna información detallada para viajes activos

## Endpoint Buscar Cancelado
`POST /api/logipath/search_guide_cancel`

=== "Flujo Explicado"
    ### 1. Verificación Inicial
    - `Validación del token JWT`
    - `Normalización de HoraInicioJornada`

    ### 2. Búsqueda del Viaje
    - `Criterios de búsqueda:`
        - `IdProgramado`
        - `RutConductor`
        - `HoraInicioJornada`
    - `Ordenamiento por Id descendente`
    - `Selección del registro más reciente`

    ### 3. Evaluación de Estado
    - `Verificación de CantidadPausa`
    - `Retorno de información según estado`

    ```json title="Estructura de buscar cancelado" linenums="1" hl_lines="2-4"
    {
        "IdProgramado": "number",
        "RutConductor": "string",
        "HoraInicioJornada": "datetime"
    }
    ```
=== "Estados y Respuestas"
    ## Respuestas
    ```json title="Éxito" linenums="1" hl_lines="2"
    {
        "message": "Viaje Cancelado",
        "IdProgramado": 12345,
        "RutConductor": "12345678-9",
        "HoraInicioJornada": "2024-01-01T12:00:00"
    }
    ``` 
    ```json title="Éxito" linenums="1" hl_lines="2"
    {
        "message": "Viaje Activo",
        "Tara": 1000,
        "Id": 12345,
        "horaConfirmacion": "2024-01-01T12:30:00",
        "latitud": -33.4569,
        "longitud": -70.6483
    }
    ``` 
    ```json title="Error" linenums="1" hl_lines="2"
    {
        "message": "Invalid request, please fill all the required fields.",
        "error": "Error específico"
    }
    ```
=== "Flujo Diagrama"
    ```mermaid
    graph TD
        A[Cliente] --> B{Validar JWT}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D[Normalizar Timestamp]
        
        D --> E[Buscar Viaje]
        E --> F{Viaje Encontrado?}
        
        F -->|No| G[Error]
        F -->|Si| H{CantidadPausa = True?}
        
        H -->|Si| I[Retornar Info Cancelación]
        H -->|No| J[Retornar Info Viaje Activo]
        
        G --> K[Respuesta Error]
        I --> L[Respuesta]
        J --> L
    ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Client
        participant API
        participant DB
        
        Client->>API: POST /search_guide_cancel
        API->>API: Validar JWT
        
        API->>API: Normalizar HoraInicioJornada
        
        API->>DB: Buscar viaje más reciente
        DB-->>API: Datos del viaje
        
        alt CantidadPausa = True
            API-->>Client: Info viaje cancelado
        else CantidadPausa = False
            API-->>Client: Info viaje activo
        end
    ```
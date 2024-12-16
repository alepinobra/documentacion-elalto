# API VIAJES TARA - Documentación

## Descripción General
Endpoint específico para actualizar la tara de un viaje y gestionar la transición a la etapa de carga.

??? info "Notas Importantes"
    - Conversión automática de zona horaria Chile a UTC
    - Notificación en tiempo real vía WebPubSub
    - Actualización selectiva de campos no nulos
    - Manejo de errores en notificaciones sin afectar la transacción principal
    - Requiere autenticación JWT sin permisos de administrador

??? warning "Consideraciones Importantes"
    - La conversión de zona horaria es crítica para la consistencia de datos
    - El WebPubSub puede fallar sin afectar la actualización del viaje
    - Los campos no proporcionados se mantienen sin cambios
    - La respuesta es exitosa incluso si falla la notificación WebPubSub
    - Se requiere manejo especial de microsegundos para compatibilidad con la base de datos

## Endpoint Post Trip Tara
`POST /api/logipath/post_trip_tara`

=== "Flujo Explicado"
    ### 1. Verificación Inicial
    - `Validación del token JWT`
    - `Normalización de timestamps (eliminación de microsegundos)`
    - `Conversión de HoraInicioJornada a UTC`

    ### 2. Actualización de Viaje Existente
    - `Búsqueda del viaje por identificadores únicos:`
        - `IdProgramado`
        - `RutConductor`
        - `HoraInicioJornada`
    - `Actualización selectiva de campos no nulos`
    - `Commit de cambios en la base de datos`

    ### 3. Notificación
    - `Envío de señal "pantallaCarga" vía WebPubSub`
    - `Manejo de errores de notificación sin afectar la transacción principal`

    ```json title="Estructura de API viajes" linenums="1" hl_lines="3-5"
    {
        "IdProgramado": "number",
        "RutConductor": "string",
        "HoraInicioJornada": "datetime",
        "TaraViaje": "number",
        "HoraConfirmacion": "datetime",
        "LatitudInicioJornada": "number?",
        "LongitudInicioJornada": "number?",
    }
    ```
=== "Estados y Respuestas"
    ## Respuestas
    ```json title="Éxito" linenums="1" hl_lines="2"
    {
        "message": "trip updated correctly"
    }
    ``` 
    ```json title="Error JWT" linenums="1" hl_lines="2"
    {
        "detail": "Invalid authentication credentials",
        "status_code": 401
    }
    ``` 
    ```json title="Error Trip" linenums="1" hl_lines="2"
    {
        "detail": "Trip not found",
        "status_code": 404
    }
    ```
=== "Flujo Diagrama"
    ```mermaid
    graph TD
        A[Cliente] --> B{Validar JWT}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D[Convertir Hora a UTC]
        
        D --> E[Normalizar Timestamps]
        E --> F[Buscar Viaje]
        
        F -->|No encontrado| G[Error 404]
        F -->|Encontrado| H[Actualizar Campos]
        
        H --> I[Commit DB]
        I --> J{Enviar WebPubSub}
        
        J -->|Error| K[Log Error]
        J -->|Success| L[Continue]
        
        K --> M[Respuesta Success]
        L --> M
    ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Client
        participant API
        participant DB
        participant WebPubSub
        
        Client->>API: POST /post_trip_tara
        API->>API: Validar JWT
        
        API->>API: Convertir hora a UTC
        API->>API: Normalizar timestamps
        
        API->>DB: Buscar viaje
        DB-->>API: Viaje encontrado
        
        API->>DB: Actualizar campos
        DB-->>API: Commit successful
        
        API->>WebPubSub: Notificar "pantallaCarga"
        
        alt Error WebPubSub
            WebPubSub-->>API: Error
            API->>API: Log error
        end
        
        API-->>Client: Respuesta exitosa
    ```
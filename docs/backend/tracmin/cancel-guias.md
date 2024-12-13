# API SAP Guides - Documentación

## Descripción General
Endpoint para la cancelación de guías de despacho integrando SAP y base de datos interna.


??? info "Notas Importantes"
    - La cancelación crea un documento de devolución (Return) en SAP
    - Se mantiene registro de todos los estados y errores en GuiaStatus
    - La cancelación es irreversible una vez completada
    - Se requiere autenticación (JWT) para acceder al endpoint

??? info "Consideraciones de Rendimiento"
    - Las consultas a SAP son asíncronas para optimizar tiempos de respuesta
    - Se implementa un sistema de retry para manejar fallos temporales de conexión
    - Se mantiene un pool de conexiones a la base de datos para mejor rendimiento

??? info "Seguridad"
    - Todas las comunicaciones con SAP son encriptadas
    - Se validan todos los inputs para prevenir inyecciones SQL y XSS
    - Los tokens y credenciales son manejados de forma segura
    - Se mantiene un log de auditoría de todas las operaciones

## Endpoint Para Cancelar Guías
`POST /api/logipath/cancel_guide`

=== "Flujo Explicado"
    ## Request Body
    ```json
    {
    "Folio": number,
    "Aprobacion": boolean
    }
    ```
    ### 1. Verificación Inicial
    - `Busca la guía en GuiaStatus por número de folio`
    - `Valida existencia de la guía en el sistema`

    ### 2. Proceso en SAP
    - `Login en SAP`
    - `Búsqueda de la guía en SAP por folio`
    - `Obtención de datos necesarios para la cancelación`
    - `Creación del documento de devolución (Return)`

    ### 3. Actualización Base de Datos
    - `Actualización del estado en GuiaStatus`
    - `Registro de la anulación`
        - `Registro de timestamp de modificación`

    ### 4. Envío de Notificación
    - `Envío de notificación WebPubSub`

=== "Estados de GuiaStatus"
    ## Estados de GuiaStatus
    - `guia_anulada`
    - `error_login_sap`
    - `error_guia_no_encontrada`
    - `error_proceso_cancelacion`

    ## Manejo de Errores
    - `Error de login SAP`
    - `Guía no encontrada en sistema interno`
    - `Guía no encontrada en SAP`
    - `Errores en proceso de cancelación`
    - `Errores de base de datos`

    ## Respuestas

    ```json title="Éxito" linenums="1" hl_lines="2"
    {
        "message": "guía anulada correctamente",
        "Folio": "12345",
        "datalink": "https://..."
    }
    ```

    ```json title="Error" linenums="1" hl_lines="2"
    {
        "message": "Error específico",
        "error": "Detalle del error"
    }
    ```
    ```bash title="Estructura del Proceso" linenums="1"
    CANCEL_GUIDE/
    │
    ├─Verificación/
    │ ├── Consulta_GuiaStatus
    │ └── Validación_Existencia
    │
    ├─SAP/
    │ ├── Login
    │ ├── Búsqueda_Guía
    │ ├── Obtención_Datos
    │ └── Crear_Return
    │
    └─Base_de_Datos/
    └── GuiaStatus
    ├── Actualización_Estado
    ├── Registro_Anulación
    └── Registro_Errores
    ```

=== "Flujo Diagrama"

    ```mermaid
    graph TD
    A[Client] --> B[POST /cancel_guide]
    B --> C{Verificar GuiaStatus}
    C -->|No Existe| D[Error: Guía no encontrada]
    C -->|Existe| E[Login SAP]
    E -->|Error| F[Registrar Error Login]
    E -->|Success| G[Buscar Guía en SAP]
    G -->|No Existe| H[Registrar Error: No encontrada]
    G -->|Existe| I[Obtener Datos]
    I --> J[Crear Return en SAP]
    J -->|Error| K[Registrar Error Cancelación]
    J -->|Success| L[Actualizar GuiaStatus]
    L --> M[Retornar Respuesta]
    %% Error paths
    D --> N[API Response Error]
    F --> N
    H --> N
    K --> N
    %% Success path
    M --> O[API Response Success]
    ```
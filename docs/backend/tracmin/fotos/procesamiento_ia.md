# API Procesamiento IA

## Descripción General
Endpoint para procesar tickets mediante Azure Form Recognizer, extrayendo información relevante y almacenándola en la base de datos.

??? info "Notas Importantes"
    - Utiliza Azure Form Recognizer para OCR
    - Validación de tickets duplicados
    - Manejo de diferentes escenarios de cliente
    - Procesamiento de peso y validaciones
    - Almacenamiento estructurado de datos extraídos

??? warning "Consideraciones Importantes"
    - El link del blob debe ser accesible
    - El modelo de IA debe estar entrenado
    - Validaciones específicas de peso
    - Manejo de casos sin romana
    - Tratamiento de errores de IA
    - Validaciones de Peso:
        ```python linenums="1"
            # Reglas de validación
            if len(peso) <= 6:
                peso = peso.replace(".", "")
            if 20000 >= int(peso) >= 35000:
                peso = "PESO NO VÁLIDO"
            else:
                peso = "PESO NO VÁLIDO"
        ```


??? tip "Mejores Prácticas"
    - Validar formato de imágenes
    - Implementar retry logic
    - Monitorear confianza de IA
    - Mantener logs detallados
    - Validar datos extraídos

## Endpoint Procesamiento IA
`POST /api/logipath/procesamiento_ia`

=== "Flujo Explicado"
    ### 1. Verificación Inicial
    - `Validación del token JWT`
    - `Búsqueda del cliente`
    - `Verificación de ticket existente`

    ### 2. Procesamiento IA
    - `Conexión a Form Recognizer`
    - `Análisis del documento`
    - `Extracción de campos`
    - `Validación de peso`

    ### 3. Almacenamiento
    - `Creación de objeto TripTicket`
    - `Persistencia en base de datos`
    - `Manejo de casos especiales`

    ```json title="Estructura de API IA" linenums="1" hl_lines="2-4"
    {
        "Cliente": "string",
        "LinkBlob": "string",
        "BlobName": "string"
    }
    ```
=== "Estados y Respuestas"
    ## Respuestas
    ```json title="Éxito Con Romana" linenums="1" hl_lines="2"
    {
        "IdTicket": "TKTxxxx-1234",
        "status_code": 200,
        "PesoConfirmado": "25000"
    }
    ``` 
    ```json title="Exito Sin Romana" linenums="1" hl_lines="2"
    {
        "IdTicket": "TKTxxxx-1234",
        "status_code": 200
    }
    ``` 
    ```json title="Error" linenums="1" hl_lines="2"
    {
        "IdTicket": "TKTxxxx-1234",
        "status_code": 409,
        "message": "Ticket ya existe"
    }
    ``` 
=== "Flujo Diagrama"
    ```mermaid
    graph TD
        A[Cliente] --> B{Validar JWT}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D{Verificar Cliente}
        
        D -->|No Encontrado| E[Crear Ticket Sin Cliente]
        D -->|Encontrado| F{Tiene Romana?}
        
        F -->|No| G[Crear Ticket Sin Romana]
        F -->|Si| H[Procesar con IA]
        
        H -->|Error| I[Crear Ticket Inválido]
        H -->|Success| J[Extraer Campos]
        
        J --> K[Validar Peso]
        K --> L[Crear Ticket]
        
        L --> M[Commit DB]
        M --> N[Return Response]
        
        E --> N
        G --> N
        I --> N
    ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Client
        participant API
        participant Form Recognizer
        participant DB
        
        Client->>API: POST /procesamiento_ia
        API->>DB: Verificar Cliente
        
        alt Cliente con Romana
            API->>Form Recognizer: Analizar Documento
            
            alt Análisis Exitoso
                Form Recognizer-->>API: Datos Extraídos
                API->>API: Validar Peso
                API->>DB: Guardar Ticket
            else Error Análisis
                API->>DB: Guardar Ticket Inválido
            end
            
        else Cliente sin Romana
            API->>DB: Guardar Ticket Sin Romana
        else Cliente No Encontrado
            API->>DB: Guardar Ticket Sin Cliente
        end
        
        DB-->>API: Commit Success
        API-->>Client: Response
    ```
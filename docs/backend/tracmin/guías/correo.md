# API Correo Guía - Documentación

## Descripción General
Sistema de notificación por correo electrónico para enviar enlaces de guías de despacho a destinatarios específicos utilizando Microsoft Graph API.

??? info "Modelos y Dependencias"
    ```python
    from fastapi import APIRouter, Request
    from fastapi.responses import JSONResponse
    from utils import graph_token
    ```

## Endpoints

=== "Envío de Correo"
    ??? success "Enviar Guía por Correo"
        ```python title="POST /api/logipath/correo_guia"
        @router.post(f"{APP_PATH}/correo_guia")
        async def enviar_correo_con_enlace(req: Request):
            """
            Envía un correo electrónico con un enlace a una guía de despacho.
            
            Args:
                req (Request): Request con key de autorización y datos JSON
                
            Body JSON:
                {
                    "Correo": "destinatario@ejemplo.com",
                    "Guia": "https://url-guia-despacho"
                }
                
            Returns:
                JSONResponse: {
                    "status": "success|error",
                    "message": str
                }
                
            Raises:
                401: Si la key es inválida
            """
        ```

=== "Ejemplos"
    ??? example "Request Exitoso"
        ```json
        {
            "Correo": "cliente@empresa.com",
            "Guia": "https://ejemplo.com/guia/123"
        }
        ```
    
    ??? example "Respuesta Exitosa"
        ```json
        {
            "status": "success",
            "message": "Email enviado correctamente"
        }
        ```
    
    ??? example "Error de Autorización"
        ```json
        {
            "message": "unauthorized"
        }
        ```

??? warning "Consideraciones de Seguridad"
    - Validación de key en headers
    - Autenticación Microsoft Graph
    - Validación de correos destinatarios
    - Sanitización de enlaces
    - Control de acceso a guías

??? tip "Mejores Prácticas"
    1. **Validación**:
        - Formato de correo electrónico
        - URLs válidas
        - Permisos de acceso
    
    2. **Monitoreo**:
        - Tasa de entrega
        - Errores de envío
        - Tiempo de respuesta
    
    3. **Mantenimiento**:
        - Rotación de tokens
        - Logging de eventos
        - Gestión de errores

??? note "Detalles Técnicos"
    | Aspecto | Especificación |
    |---------|----------------|
    | Método | POST |
    | Auth | Header Key |
    | Formato | JSON |
    | Email From | notificaciones@tracmin.cl |
    | Retry | No |
    | Timeout | 30s |

=== "Códigos de Estado"
    | Código | Descripción |
    |--------|-------------|
    | 200 | Email enviado correctamente |
    | 401 | No autorizado |
    | 500 | Error interno |
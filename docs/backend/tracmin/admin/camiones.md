# API Camiones - Documentación

## Descripción General
Sistema de gestión de flota que permite crear, modificar y consultar información de camiones y acoplados en el sistema.

??? info "Notas Importantes"
    - Sistema CRUD para gestión de camiones
    - Requiere autenticación JWT para operaciones
    - Modelo principal:
        ```python title="modelos"
        class DeviceAdminRequest(BaseModel):
            Patente: Optional[str] = None
            Id: Optional[int] = None
            CodigoTransportista: Optional[str] = None
            Codigo: Optional[str] = None
            Activo: Optional[bool] = None
            Acoplado: bool = False
        ```

??? warning "Consideraciones de Seguridad"
    - Validar permisos de administrador
    - Validar datos únicos (Patente)
    - Manejar errores de integridad
    - Validar relación con transportistas
    - Proteger información sensible de la flota

??? tip "Mejores Prácticas"
    - Validar formato de patentes
    - Verificar existencia de transportista
    - Implementar logging de cambios
    - Mantener historial de modificaciones
    - Validar estado de activo/inactivo

## Endpoints Camiones
- `POST /api/logipath/get_trucks`
- `POST /api/logipath/admin/devices`

=== "Endpoints"
    ??? success "Consultar Camiones"
        ```python title="POST /api/logipath/get_trucks"
        @router.post(f"{APP_PATH}/get_trucks")
        async def get_trucks(sec: Annotated[str, Depends(security_scheme)]):
            """
            Retorna lista de camiones y acoplados.
            
            Args:
                sec (str): Token JWT de autenticación
                
            Returns:
                dict: {
                    "results": [Lista de camiones],
                    "acoplados": [Lista de acoplados]
                }
                
            Raises:
                HTTPException: Si el token es inválido
            """
        ```
    
    ??? success "Agregar/Modificar Camión"
        ```python title="POST /api/logipath/admin/devices"
        @router.post(f"{APP_PATH}/admin/devices")
        async def add_device(
            sec: Annotated[str, Depends(security_scheme)], 
            req: DeviceAdminRequest
        ):
            """
            Crea o modifica un camión en el sistema.
            
            Args:
                sec (str): Token JWT de administrador
                req (DeviceAdminRequest): Datos del camión
                
            Returns:
                dict: {"message": "truck added/edited correctly"}
                
            Raises:
                HTTPException: 
                    - 400: Si la patente ya existe
                    - 401: Si el token es inválido
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Cliente
        participant API
        participant DB
        
        alt Consultar Camiones
            Cliente->>API: POST /get_trucks
            API->>API: Validar JWT
            API->>DB: Query Camiones
            DB-->>API: Listas
            API-->>Cliente: Results + Acoplados
        
        else Crear/Modificar Camión
            Cliente->>API: POST /admin/devices
            API->>API: Validar JWT Admin
            API->>DB: Verificar Patente
            
            alt Modificación
                API->>DB: Actualizar Camión
                DB-->>API: Confirmación
                API-->>Cliente: Success
            else Creación
                alt Patente Existe
                    API-->>Cliente: Error 400
                else
                    API->>DB: Crear Camión
                    DB-->>API: Confirmación
                    API-->>Cliente: Success
                end
            end
        end
    ```

=== "Ejemplos de Respuesta"
    ??? example "Consulta Exitosa"
        ```json
        {
            "results": [
                {
                    "Id": 1,
                    "Patente": "ABCD12",
                    "CodigoTransportista": "TRANS1",
                    "Codigo": "CAM001",
                    "Activo": true,
                    "Acoplado": false
                }
            ],
            "acoplados": [
                {
                    "Id": 2,
                    "Patente": "WXYZ34",
                    "CodigoTransportista": "TRANS1",
                    "Codigo": "ACO001",
                    "Activo": true,
                    "Acoplado": true
                }
            ]
        }
        ```
    
    ??? example "Error Patente Duplicada"
        ```json
        {
            "detail": {
                "message": "Patente ya existe, por favor elige una Patente diferente"
            }
        }
        ```

=== "Códigos de Estado"
    - `200`: Operación exitosa
    - `400`: Error de validación (patente duplicada)
    - `401`: No autorizado
    - `403`: Permisos insuficientes
    - `500`: Error interno del servidor

??? note "Validaciones Importantes"
    - Formato de patente según normativa chilena
    - Existencia del código de transportista
    - Estado activo/inactivo coherente
    - Relación correcta entre camión y acoplado
    - Unicidad de códigos internos
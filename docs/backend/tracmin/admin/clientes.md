# API Clientes - Documentación

## Descripción General
Sistema de gestión de clientes que permite crear, modificar y consultar información de clientes en el sistema.

??? info "Notas Importantes"
    - Sistema CRUD para gestión de clientes
    - Requiere autenticación JWT para operaciones
    - Modelo principal:
        ```python title="modelos"
        class clientAdminRequest(BaseModel):
            Id: Optional[int] = None
            Rut: Optional[str] = None
            Codigo: Optional[str] = None
            Latitud: Optional[float] = None
            Longitud: Optional[float] = None
            Direccion: Optional[str] = None
            Comuna: Optional[str] = None
            Region: Optional[str] = None
            Romana: Optional[bool] = None
            Activo: Optional[bool] = None
            Mod: Optional[bool] = None
        ```

??? warning "Consideraciones de Seguridad"
    - Validar permisos de administrador
    - Validar datos únicos (Rut)
    - Manejar errores de integridad
    - Validar coordenadas geográficas
    - Proteger información sensible del cliente

??? tip "Mejores Prácticas"
    - Validar formato de Rut
    - Validar coordenadas dentro de rangos válidos
    - Implementar logging de cambios
    - Mantener historial de modificaciones
    - Validar datos de dirección

## Endpoints Clientes
- `POST /api/logipath/get_clients`
- `POST /api/logipath/admin/clients`

=== "Endpoints"
    ??? success "Consultar Clientes"
        ```python title="POST /api/logipath/get_clients"
        @router.post(f"{APP_PATH}/get_clients")
        async def get_clients(sec: Annotated[str, Depends(security_scheme)]):
            """
            Retorna lista de todos los clientes registrados.
            
            Args:
                sec (str): Token JWT de autenticación
                
            Returns:
                dict: {
                    "results": [Lista de clientes]
                }
                
            Raises:
                HTTPException: Si el token es inválido
            """
        ```
    
    ??? success "Agregar/Modificar Cliente"
        ```python title="POST /api/logipath/admin/clients"
        @router.post(f"{APP_PATH}/admin/clients")
        async def add_client(
            sec: Annotated[str, Depends(security_scheme)], 
            req: clientAdminRequest
        ):
            """
            Crea o modifica un cliente en el sistema.
            
            Args:
                sec (str): Token JWT de administrador
                req (clientAdminRequest): Datos del cliente
                
            Returns:
                dict: {"message": "usuario agregado/editado correctamente"}
                
            Raises:
                HTTPException: 
                    - 400: Si el Rut ya existe
                    - 401: Si el token es inválido
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Cliente
        participant API
        participant DB
        
        alt Consultar Clientes
            Cliente->>API: POST /get_clients
            API->>API: Validar JWT
            API->>DB: Query Clientes
            DB-->>API: Lista Clientes
            API-->>Cliente: Results
        
        else Crear/Modificar Cliente
            Cliente->>API: POST /admin/clients
            API->>API: Validar JWT Admin
            API->>DB: Verificar Rut
            
            alt Modificación
                API->>DB: Actualizar Cliente
                DB-->>API: Confirmación
                API-->>Cliente: Success
            else Creación
                alt Rut Existe
                    API-->>Cliente: Error 400
                else
                    API->>DB: Crear Cliente
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
                    "Rut": "12345678-9",
                    "Codigo": "CLI1",
                    "Latitud": -33.4569,
                    "Longitud": -70.6483,
                    "Direccion": "Ejemplo 123",
                    "Comuna": "Santiago",
                    "Region": "Metropolitana",
                    "Romana": true,
                    "Activo": true
                }
            ]
        }
        ```
    
    ??? example "Error Rut Duplicado"
        ```json
        {
            "detail": {
                "message": "Cliente ya existe, por favor elige un Rut diferente"
            }
        }
        ```
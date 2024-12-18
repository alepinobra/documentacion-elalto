# API Transportistas - Documentación

## Descripción General
Sistema de gestión de transportistas que permite crear y consultar información de empresas transportistas en el sistema.

??? info "Notas Importantes"
    - Sistema CRUD para gestión de transportistas
    - Requiere autenticación JWT para todas las operaciones
    - Modelo principal:
        ```python title="modelos"
        class TransportistaRequest(BaseModel):
            NombreTransportista: str
            Rut: str
            Codigo: str
            Activo: bool
        ```

??? warning "Consideraciones de Seguridad"
    - Validar permisos de administrador
    - Validar datos únicos (Rut)
    - Manejar errores de integridad
    - Proteger rutas sensibles
    - Validar formato de Rut

??? tip "Mejores Prácticas"
    - Validar inputs antes de procesar
    - Manejar errores de forma consistente
    - Implementar logging de operaciones
    - Mantener consistencia en respuestas
    - Documentar códigos de error

## Endpoints Transportistas
- `POST /api/logipath/transportistas`
- `POST /api/logipath/admin/transportistas`

=== "Endpoints"
    ??? success "Consultar Transportistas"
        ```python title="POST /api/logipath/transportistas"
        @router.post(f"{APP_PATH}/transportistas")
        async def return_users(sec: Annotated[str, Depends(security_scheme)]):
            """
            Retorna lista de todos los transportistas registrados.
            
            Args:
                sec (str): Token JWT de autenticación
                
            Returns:
                dict: {
                    "message": "queried successfully",
                    "objects": [Lista de transportistas]
                }
                
            Raises:
                HTTPException: Si el token es inválido o hay error en la consulta
            """
        ```
    
    ??? success "Agregar Transportista"
        ```python title="POST /api/logipath/admin/transportistas"
        @router.post(f"{APP_PATH}/admin/transportistas")
        async def add_transportista(
            sec: Annotated[str, Depends(security_scheme)], 
            req: TransportistaRequest
        ):
            """
            Crea un nuevo transportista en el sistema.
            
            Args:
                sec (str): Token JWT de administrador
                req (TransportistaRequest): Datos del transportista
                
            Returns:
                dict: {"message": "transportista agregado correctamente"}
                
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
        
        alt Consultar Transportistas
            Cliente->>API: POST /transportistas
            API->>API: Validar JWT
            
            alt JWT Válido
                API->>DB: Query Transportistas
                DB-->>API: Lista Transportistas
                API-->>Cliente: Success + Lista
            else JWT Inválido
                API-->>Cliente: Error 401
            end
        
        else Crear Transportista
            Cliente->>API: POST /admin/transportistas
            API->>API: Validar JWT Admin
            API->>DB: Verificar Rut Existente
            
            alt Rut Existe
                DB-->>API: Transportista Existente
                API-->>Cliente: Error 400
            else Rut No Existe
                API->>DB: Crear Transportista
                DB-->>API: Confirmación
                API-->>Cliente: Success
            end
        end
    ```

=== "Ejemplos de Respuesta"
    ??? example "Consulta Exitosa"
        ```json
        {
            "message": "queried successfully",
            "objects": [
                {
                    "NombreTransportista": "Empresa 1",
                    "Rut": "12345678-9",
                    "Codigo": "EMP1",
                    "Activo": true
                }
            ]
        }
        ```
    
    ??? example "Error Rut Duplicado"
        ```json
        {
            "detail": {
                "message": "Rut ya existe, por favor elige un Rut diferente"
            }
        }
        ```
# API Users - Documentación

## Descripción General
Sistema de gestión de usuarios y transportistas que permite crear, modificar y consultar información de usuarios del sistema.

??? info "Notas Importantes"
    - Sistema CRUD para gestión de usuarios
    - Requiere autenticación JWT para operaciones administrativas
    - Modelos principales:
        ```python title="modelos"
        class UserAdminRequest(BaseModel):
            Rut: Optional[str] = None
            Id: Optional[int] = None
            CodigoTransportista: Optional[str] = None
            Nombre: Optional[str] = None
            Apellido: Optional[str] = None
            Telefono: Optional[str] = None
            Email: Optional[str] = None
            Activo: Optional[bool] = None
            Admin: Optional[int] = None
            Contrasena: Optional[str] = None
            Mod: Optional[bool] = None
        ```

??? warning "Consideraciones de Seguridad"
    - Validar permisos de administrador
    - Encriptar contraseñas
    - Validar datos únicos (Rut)
    - Manejar errores de integridad
    - Proteger rutas sensibles

??? tip "Mejores Prácticas"
    - Validar inputs
    - Manejar errores consistentemente
    - Documentar respuestas de error
    - Implementar logging
    - Mantener consistencia en respuestas

## Endpoints Users
- `POST /api/logipath/admin/users`
- `POST /api/logipath/users`

=== "Endpoints"
    ??? success "Agregar/Modificar Usuario"
        ```python title="POST /api/logipath/admin/users"
        @router.post(f"{APP_PATH}/admin/users")
        async def add_user(sec: Annotated[str, Depends(security_scheme)], req: UserAdminRequest):
            """
            Crea o modifica un usuario en el sistema.
            
            Args:
                sec (str): Token JWT de administrador
                req (UserAdminRequest): Datos del usuario
                
            Returns:
                dict: Mensaje de confirmación
                
            Raises:
                HTTPException: Si el Rut ya existe o hay errores de validación
            """
        ```
    
    ??? success "Consultar Usuarios"
        ```python title="POST /api/logipath/users"
        @router.post(f"{APP_PATH}/users")
        async def return_users(req: Request):
            """
            Retorna lista de usuarios del sistema.
            
            Args:
                req (Request): Request con key de autorización
                
            Returns:
                dict: Lista de usuarios con sus datos
                
            Raises:
                HTTPException: Si la key es inválida
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Cliente
        participant API
        participant DB
        
        alt Crear/Modificar Usuario
            Cliente->>API: POST /admin/users
            API->>API: Validar JWT Admin
            API->>DB: Verificar Rut Existente
            
            alt Rut Existe
                DB-->>API: Usuario Existente
                alt Modificación
                    API->>DB: Actualizar Usuario
                    DB-->>API: Confirmación
                    API-->>Cliente: Success
                else Creación
                    API-->>Cliente: Error 400
                end
            else Rut No Existe
                API->>DB: Crear Usuario
                DB-->>API: Confirmación
                API-->>Cliente: Success
            end
        
        else Consultar Usuarios
            Cliente->>API: POST /users
            API->>API: Validar Key
            
            alt Key Válida
                API->>DB: Query Usuarios
                DB-->>API: Lista Usuarios
                API-->>Cliente: Lista Usuarios
            else Key Inválida
                API-->>Cliente: Unauthorized
            end
        end
    ```
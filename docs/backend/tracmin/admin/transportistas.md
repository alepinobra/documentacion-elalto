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
- `POST /api/logipath/crear_transportista`
- `POST /api/logipath/validar_bbdd`

=== "Endpoints"

    ??? success "Consultar Transportistas"
        ```python title="POST /api/logipath/transportistas"
        @router.post(f"{APP_PATH}/transportistas")
        async def return_users(sec: Annotated[str, Depends(security_scheme)]):
            """
            Retorna lista de todos los transportistas registrados.
            Returns:
                dict: {
                    "message": "queried successfully",
                    "objects": [Lista de transportistas]
                }
            """
            # ...
            return {
                "message": "queried successfully",
                "objects": [/* ... */]
            }
        ```

    ??? success "Agregar Transportista, Usuarios y Camiones (Flujo Completo)"
        ```python title="POST /api/logipath/crear_transportista"
        @router.post(f"{APP_PATH}/crear_transportista")
        async def add_transportistas_flujo(
            sec: Annotated[str, Depends(security_scheme)],
            req: TransportistaFlujoRequest
        ):
            """
            Agrega un transportista, sus usuarios y camiones en un solo flujo.
            Valida unicidad y consistencia de datos antes de insertar.
            Returns:
                dict: {
                    "success": true,
                    "message": "Datos validados correctamente",
                    "resumen": {...},
                    "datos_validados": {...}
                }
            """
            # ...
            return {
                "success": True,
                "message": "Datos validados correctamente",
                "resumen": {/* ... */},
                "datos_validados": {/* ... */}
            }
        ```

    ??? success "Validar existencia en BBDD (Transportista, Usuario, Camión)"
        ```python title="POST /api/logipath/validar_bbdd"
        @router.post(f"{APP_PATH}/validar_bbdd")
        async def validaciones_datos_bbdd(sec: Annotated[str, Depends(security_scheme)], req: validacion):
            """
            Valida si existe un transportista, usuario o camión en la base de datos según el tipo y código/RUT entregado.
            Returns:
                dict: {
                    "message": "El transportista ya existe" | "No existe",
                    "existe": true | false
                }
            """
            # ...
            return {
                "message": "El transportista ya existe",
                "existe": True
            }
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
            API->>DB: Query Transportistas
            DB-->>API: Lista Transportistas
            API-->>Cliente: Results
        
        else Alta Transportista/Usuarios/Camiones
            Cliente->>API: POST /crear_transportista
            API->>API: Validar JWT Admin
            API->>DB: Validar datos
            API->>DB: Verificar unicidad
            API->>DB: Insertar transportista
            API->>DB: Insertar usuarios
            API->>DB: Insertar camiones
            DB-->>API: Confirmación
            API-->>Cliente: Success
        
        else Validar existencia en BBDD
            Cliente->>API: POST /validar_bbdd
            API->>API: Validar JWT
            API->>DB: Buscar por tipo y código
            DB-->>API: Resultado
            API-->>Cliente: Existe/No existe
        end
    ```

=== "Ejemplo de Respuesta Exitosa"
    ??? example "Alta Exitosa"
        ```json
        {
            "success": true,
            "message": "Datos validados correctamente",
            "resumen": {
                "transportista": "TRANSPORTES EJEMPLO",
                "total_usuarios": 2,
                "usuarios_admin": 1,
                "total_camiones": 3,
                "camiones_acoplados": 1
            },
            "datos_validados": {
                "transportista": {
                    "rut": "12345678-9",
                    "nombre": "TRANSPORTES EJEMPLO",
                    "codigo": "TRX",
                    "activo": true
                },
                "usuarios": [
                    {
                        "rut": "11111111-1",
                        "nombre_completo": "Juan Perez",
                        "telefono": "987654321",
                        "email": "juan@ejemplo.cl",
                        "activo": true,
                        "admin": true,
                        "rol": "Transportista"
                    }
                ],
                "camiones": [
                    {
                        "codigo": "CAM001",
                        "patente": "AB1234",
                        "activo": true,
                        "acoplado": false
                    }
                ]
            }
        }
        ```
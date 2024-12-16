# API Seguridad - Documentación

## Descripción General
Sistema de autenticación y autorización basado en JWT (JSON Web Tokens) con manejo de roles y permisos específicos.

??? info "Notas Importantes"
    - Sistema de autenticación y autorización basado en JWT (JSON Web Tokens) con manejo de roles y permisos específicos.
    - configuracion basica: 
        ```python title="configuracion basica"
        SECRET_KEY = os.environ["HASH_KEY"]
        ALGORITHM = "HS256"
        ACCESS_TOKEN_EXPIRE_MINUTES = 1440  # 24 horas

        security_scheme = OAuth2PasswordBearer(tokenUrl=f"{APP_PATH}/login")
        pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
        ```
    - modelos token:
        ```python title="modelos token"
        class Token(BaseModel):
            access_token: str
            token_type: str

        class TokenData(BaseModel):
            nombre: str | None = None
        ```

??? warning "Consideraciones de Seguridad"
    - Mantener SECRET_KEY segura (Key Vault)
    - Usar HTTPS
    - Implementar rate limiting
    - Rotar tokens regularmente
    - Validar inputs
    - Mantener dependencias actualizadas

??? tip "Mejores Prácticas"
    - Usar variables de entorno
    - Implementar logging de seguridad
    - Manejar errores consistentemente
    - Validar permisos granularmente
    - Mantener tokens con scope mínimo necesario

## Endpoints Seguridad
- `POST /api/logipath/login`
- `POST /api/logipath/token_activo_admin`
- `POST /api/logipath/token_activo_user`

=== "Funciones"
    ??? success "Verificación de Contraseña"
        ```python title="Verificación de Contraseña"
            def verify_password(plain_password, hashed_password):
            """
            Verifica si una contraseña en texto plano coincide con su hash.
            
            Args:
                plain_password (str): Contraseña en texto plano
                hashed_password (str): Hash de la contraseña
                
            Returns:
                bool: True si coinciden, False si no
            """
        ```
    ??? success "Generación de Hash de Contraseña"
        ```python title="Generación de Hash de Contraseña"
            def get_password_hash(password):
            """
            Genera un hash seguro de una contraseña.
                
            Args:
                password (str): Contraseña en texto plano
                
            Returns:
                str: Hash de la contraseña
            """
        ```
    ??? success "Decodificación y Validación de Token JWT"
        ```python title="Decodificación y Validación de Token JWT"
            def decode_jwt(token: str, admin: int) -> Any:
            """
            Decodifica y valida un token JWT.
                
            Args:
                token (str): Token JWT a validar
                admin (int): Nivel de privilegios requerido (0: usuario, 1-2: admin)
                
            Returns:
                TokenData: Datos del token decodificado
                
            Raises:
                HTTPException: Si el token es inválido o los privilegios son insuficientes
            """
        ```
    ??? success "Generación de Token JWT"
        ```python title="Generación de Token JWT"
            def create_access_token(data: dict, expires_delta: timedelta | None = None):
            """
            Genera un nuevo token JWT.
            
            Args:
                data (dict): Datos a incluir en el token
                expires_delta (timedelta, optional): Tiempo de expiración
                
            Returns:
                str: Token JWT generado
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
    participant Cliente
    participant API
    participant DB
    
    Cliente->>API: POST /login
    API->>DB: Verificar Credenciales
    
    alt Credenciales Válidas
        DB-->>API: Datos Usuario
        API->>API: Generar JWT
        API-->>Cliente: Token
    else Credenciales Inválidas
        API-->>Cliente: Error 401
    end
    
    Cliente->>API: Request con Token
    API->>API: Decodificar JWT
    
    alt Token Válido
        API->>API: Verificar Permisos
        alt Permisos Suficientes
            API-->>Cliente: Respuesta
        else Permisos Insuficientes
            API-->>Cliente: Error 403
        end
    else Token Inválido
        API-->>Cliente: Error 401
    end
    ```
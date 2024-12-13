# Configuración del Entorno de Desarrollo en Windows

## Prerequisitos

=== "Instalaciones"

    ### 1. Azure CLI
    ```bash title="Descargar e instalar Azure CLI" linenums="1"
    https://aka.ms/installazurecliwindows
    ```

    ### 2. Azure Functions Core Tools
    ```bash title="Descargar e instalar Azure Functions Core Tools" linenums="1"
    https://go.microsoft.com/fwlink/?linkid=2174087
    ```

    ### 3. Visual C++ Redistributable
    ```bash title="Descargar e instalar Visual C++ Redistributable" linenums="1"
    https://aka.ms/vs/17/release/vc_redist.x64.exe
    ```

    ### 4. Microsoft ODBC Driver 17
    ```bash title="Descargar e instalar Microsoft ODBC Driver 17" linenums="1"
    https://go.microsoft.com/fwlink/?linkid=2187214
    ```

=== "Configuración del repositorio"

    1. **Autenticación en Azure**

    ```powershell title="Iniciar sesión en Azure" linenums="1"
    az login
    ```

    2. **Configuración del Repositorio**
    
    ```powershell title="Clonar el repositorio" linenums="1"
    git clone link_del_repositorio
    ```

    ```powershell title="Copiar archivo localonly" linenums="1"
    cp localonly .
    ```

    3. **Entorno Virtual Python**

    ```powershell title="Crear y activar entorno virtual" linenums="1"
    python -m venv venv
    .\venv\Scripts\activate
    pip install -r requirements.txt
    ```

    4. **Configuración de Azure Functions**

    ```powershell title="Configurar Azure Functions" linenums="1"
    func azure functionapp fetch-app-settings elalto
    ```

    5. **Crear certificados HTTPS**

    ```powershell title="Generar certificados" linenums="1"
    $cert = New-SelfSignedCertificate -Subject localhost -DnsName localhost -FriendlyName "Functions Development" -KeyUsage DigitalSignature -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.1")
    Export-PfxCertificate -Cert $cert -FilePath certificate.pfx -Password (ConvertTo-SecureString -String 'password' -Force -AsPlainText)
    ```

=== "Levantar Backend de forma local"

    ### Configuración
    
    ```powershell title="Iniciar el backend" linenums="1"
    func start --functions AppLogiPath --useHttps --cert certificate.pfx --password 'password'
    ```
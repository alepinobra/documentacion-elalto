# Configuración del Entorno de Desarrollo en Linux

## Prerequisitos

=== "Instalaciones"

    ### 1. Azure CLI

    ```bash title="Instalar Azure CLI" linenums="1"
    curl -sL https://aka.ms/InstallAzureCLIDeb | sudo bash
    ```

    ### 2. Azure Functions Core Tools

    ```bash title="Instalar Azure Functions Core Tools" linenums="1"
    curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg
    sudo mv microsoft.gpg /etc/apt/trusted.gpg.d/microsoft.gpg
    sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/microsoft-ubuntu-$(lsb_release -cs)-prod $(lsb_release -cs) main" > /etc/apt/sources.list.d/dotnetdev.list'
    sudo apt-get update
    sudo apt-get install azure-functions-core-tools-4
    ```

    ### 3. Microsoft ODBC Driver 17

    ```bash title="Instalar Microsoft ODBC Driver 17" linenums="1"
    curl https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
    sudo curl https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/prod.list > /etc/apt/sources.list.d/mssql-release.list
    sudo apt-get update
    sudo ACCEPT_EULA=Y apt-get install -y msodbcsql17
    ```

=== "Configuración del repositorio"

    1. **Autenticación en Azure**

    ```bash title="Iniciar sesión en Azure" linenums="1"
    az login
    ```
    2. **Configuración del Repositorio**
    
    ```bash title="Clonar el repositorio" linenums="1"
    git clone link_del_repositorio
    ```

    ```bash title="Equipo te tiene que dar el archivo `localonly`" linenums="1"
    cp Tracmin/localonly .
    ```

    3. **Entorno Virtual Python**

    ```bash title="Crear y activar entorno virtual" linenums="1"
    python -m venv venv
    source venv/bin/activate
    pip install -r requirements.txt
    ```

    4. **Configuración de Azure Functions**

    ```bash title="Configurar Azure Functions" linenums="1"
    func azure functionapp fetch-app-settings elalto
    ```

    4. **Levantar Backend de forma local**

    ```bash title="Desarrollo general" linenums="1"
    func start --functions AppLogiPath --useHttps
    ```

=== "Levantar Backend de forma local"

    ### Configuración
    
    ```bash title="Ejemplo para llamada Postman" linenums="1"
    - URL: `http://localhost:7071/admin/functions/CargasLoadriteRaw`
    - Método: POST, GET, etc
    - Headers: segun el caso
    ```


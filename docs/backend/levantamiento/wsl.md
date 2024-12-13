# Guía de Instalación y Configuración de WSL

## 1. Habilitar Características de Windows

Activar en "Windows Features":
- Virtual Machine Platform
- Windows Hypervisor Platform

## 2. Instalar WSL

```powershell title="Instalar WSL" linenums="1"
wsl --install
```

## 3. Configuración Inicial

### Comandos básicos
```bash title="Comandos útiles" linenums="1"
pwd     # Ver directorio actual
mkdir   # Crear carpeta
code .  # Abrir Visual Studio Code
```

### Instalar curl
```bash title="Instalar curl" linenums="1"
sudo apt-get install curl
```

## 4. Instalación de Herramientas

=== "NVM y Node.js"

    ```bash title="Instalar NVM" linenums="1"
    curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.3/install.sh | bash
    ```

    ```bash title="Instalar y usar Node.js" linenums="1"
    nvm install node    # Instala la última versión
    nvm use <version>   # Usar una versión específica
    ```

=== "Miniconda"

    ```bash title="Descargar Miniconda" linenums="1"
    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
    ```

    ```bash title="Instalar Miniconda" linenums="1"
    sh ./Miniconda3-latest-Linux-x86_64.sh
    ```

    ```bash title="Instalar versión específica de Python" linenums="1"
    conda install python=<version>
    ```

    Nota: Cuando ves `(base)` en tu terminal, indica que estás en el entorno base de conda.

=== "Azure Functions Core Tools"

    ```bash title="Instalar Azure Functions Core Tools" linenums="1"
    curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg
    sudo mv microsoft.gpg /etc/apt/trusted.gpg.d/microsoft.gpg
    
    sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/microsoft-ubuntu-$(lsb_release -cs)-prod $(lsb_release -cs) main" > /etc/apt/sources.list.d/dotnetdev.list'
    
    sudo apt-get update
    sudo apt-get install azure-functions-core-tools-4
    ```

## 5. Configuración de Visual Studio Code

1. Instalar la extensión "WSL" en VS Code
2. Usar el comando `code .` desde WSL para abrir VS Code con integración WSL

## 6. Azure CLI y ODBC

Azure CLI y ODBC deberían estar disponibles por defecto en WSL. Para verificar:

```bash title="Verificar Azure CLI" linenums="1"
az --version
az login    # Para iniciar sesión en Azure
```

## Notas Adicionales

- Para ejecutar funciones localmente, usar los comandos de Azure Functions como se describe en la guía de Windows
- WSL proporciona un entorno Linux completo dentro de Windows
- Se puede acceder a los archivos de Windows desde WSL en `/mnt/c/`

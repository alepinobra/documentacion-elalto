# 🚀 Guía de Configuración: Entorno de Desarrollo

## 💻 Requisitos del Sistema

=== "Windows"
    | Componente | Link |
    |------------|------|
    | Azure CLI | [Descargar ⬇️](https://aka.ms/installazurecliwindows) |
    | Functions Core Tools | [Descargar ⬇️](https://go.microsoft.com/fwlink/?linkid=2174087) |
    | VC++ Redistributable | [Descargar ⬇️](https://aka.ms/vs/17/release/vc_redist.x64.exe) |
    | ODBC Driver 17 | [Descargar ⬇️](https://go.microsoft.com/fwlink/?linkid=2187214) |

=== "Linux"
    ```bash
    # 1. Azure CLI
    curl -sL https://aka.ms/InstallAzureCLIDeb | sudo bash

    # 2. Functions Core Tools
    curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg
    sudo mv microsoft.gpg /etc/apt/trusted.gpg.d/microsoft.gpg
    sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/microsoft-ubuntu-$(lsb_release -cs)-prod $(lsb_release -cs) main" > /etc/apt/sources.list.d/dotnetdev.list'
    sudo apt-get update
    sudo apt-get install azure-functions-core-tools-4

    # 3. ODBC Driver
    curl https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
    sudo curl https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/prod.list > /etc/apt/sources.list.d/mssql-release.list
    sudo apt-get update
    sudo ACCEPT_EULA=Y apt-get install -y msodbcsql17
    ```

## 🛠️ Configuración del Entorno

### 1️⃣ Autenticación Azure

=== "Windows & Linux"
    ```bash
    # Iniciar sesión en Azure
    az login
    ```

### 2️⃣ Configuración del Repositorio

=== "Windows"
    ```powershell
    # Clonar repositorio
    git clone link_del_repositorio
    
    # Copiar archivo local
    cp localonly .
    
    # Configurar entorno Python
    python -m venv venv
    .\venv\Scripts\activate
    pip install -r requirements.txt
    ```

=== "Linux"
    ```bash
    # Clonar repositorio
    git clone link_del_repositorio
    
    # Copiar archivo local
    cp Tracmin/localonly .
    
    # Configurar entorno Python
    python -m venv venv
    source venv/bin/activate
    pip install -r requirements.txt
    ```

### 3️⃣ Configuración Azure Functions

=== "Windows & Linux"
    ```bash
    # Obtener configuración
    func azure functionapp fetch-app-settings elalto
    ```

### 4️⃣ Certificados HTTPS

=== "Windows"
    ```powershell
    # Generar certificado
    $cert = New-SelfSignedCertificate `
        -Subject localhost `
        -DnsName localhost `
        -FriendlyName "Functions Development" `
        -KeyUsage DigitalSignature `
        -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.1")
    
    # Exportar certificado
    Export-PfxCertificate `
        -Cert $cert `
        -FilePath certificate.pfx `
        -Password (ConvertTo-SecureString -String 'password' -Force -AsPlainText)
    ```

=== "Linux"
    ```bash
    # HTTPS habilitado por defecto
    # No requiere configuración adicional
    ```

## 🚦 Iniciar el Backend

=== "Windows"
    ```powershell
    func start --functions AppLogiPath `
               --useHttps `
               --cert certificate.pfx `
               --password 'password'
    ```

=== "Linux"
    ```bash
    func start --functions AppLogiPath --useHttps
    ```

## 🔍 Pruebas

??? example "Ejemplo Postman"
    ```plaintext
    URL: http://localhost:7071/admin/functions/CargasLoadriteRaw
    Método: POST, GET, etc.
    Headers: Según el caso
    ```

## ✅ Verificación

??? check "Lista de Verificación"
    - [ ] Herramientas instaladas
    - [ ] Azure CLI configurado
    - [ ] Repositorio clonado
    - [ ] Entorno virtual activo
    - [ ] Dependencias instaladas
    - [ ] Backend funcionando

## ⚠️ Solución de Problemas

??? warning "Problemas Comunes"
    | Problema | Windows | Linux |
    |----------|---------|-------|
    | Permisos | Ejecutar como admin | Usar sudo |
    | Puertos | Verificar 7071 | Verificar 7071 |
    | SSL | Regenerar certificados | Verificar configuración |
    | Dependencias | Actualizar pip | Actualizar apt |

## 📚 Recursos

??? info "Enlaces Útiles"
    - [Azure Functions](https://docs.microsoft.com/azure/azure-functions/)
    - [Python en Azure](https://docs.microsoft.com/azure/python/)
    - [Troubleshooting](https://docs.microsoft.com/azure/azure-functions/functions-recover-storage-account)
# 🐧 WSL: Linux en Windows

## 🎯 ¿Por qué WSL?

??? info "Ventajas de WSL"
    - Entorno Linux nativo en Windows
    - Mejor rendimiento para desarrollo
    - Compatibilidad con herramientas Linux
    - Integración perfecta con VS Code
    - Acceso a archivos de Windows

## 🚀 Instalación y Configuración

### 1️⃣ Preparación de Windows

??? tip "Activar Características"
    1. Abrir "Windows Features"
    2. Activar:
        - ✅ Virtual Machine Platform
        - ✅ Windows Hypervisor Platform
        - ✅ Windows Subsystem for Linux

### 2️⃣ Instalación WSL

=== "Método Rápido"
    ```powershell
    wsl --install
    ```

=== "Instalación Manual"
    ```powershell
    # 1. Habilitar WSL
    dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
    
    # 2. Habilitar Plataforma Virtual
    dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
    
    # 3. Reiniciar Windows
    
    # 4. Establecer WSL 2 como predeterminado
    wsl --set-default-version 2
    ```

## 🛠️ Herramientas Esenciales

### 3️⃣ Desarrollo

=== "Node.js (NVM)"
    ```bash
    # Instalar NVM
    curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.3/install.sh | bash
    
    # Instalar Node.js
    nvm install node
    ```

=== "Python (Miniconda)"
    ```bash
    # Descargar Miniconda
    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
    
    # Instalar
    sh ./Miniconda3-latest-Linux-x86_64.sh
    ```

=== "Azure Tools"
    ```bash
    # Azure Functions Core Tools
    curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg
    sudo mv microsoft.gpg /etc/apt/trusted.gpg.d/microsoft.gpg
    sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/microsoft-ubuntu-$(lsb_release -cs)-prod $(lsb_release -cs) main" > /etc/apt/sources.list.d/dotnetdev.list'
    sudo apt-get update
    sudo apt-get install azure-functions-core-tools-4
    ```

## 📝 VS Code Integration

??? example "Configuración VS Code"
    1. Instalar extensión "WSL"
    2. Comandos útiles:
        ```bash
        # Abrir VS Code
        code .
        
        # Abrir carpeta actual
        code ./mi-proyecto
        ```

## 🔍 Comandos Útiles

??? tip "Comandos Básicos"
    | Comando | Descripción |
    |---------|-------------|
    | `pwd` | Directorio actual |
    | `ls` | Listar archivos |
    | `cd` | Cambiar directorio |
    | `mkdir` | Crear carpeta |
    | `touch` | Crear archivo |

??? tip "Comandos WSL"
    | Comando | Descripción |
    |---------|-------------|
    | `wsl --list` | Ver distros instaladas |
    | `wsl --status` | Estado de WSL |
    | `wsl --shutdown` | Apagar WSL |
    | `wsl --update` | Actualizar WSL |

## ⚙️ Acceso a Archivos

??? info "Sistema de Archivos"
    - **Windows desde WSL**: `/mnt/c/`
    - **WSL desde Windows**: `\\wsl$\Ubuntu\home\usuario`

## ✅ Verificación

??? check "Lista de Verificación"
    - [ ] WSL instalado y funcionando
    - [ ] VS Code integrado
    - [ ] Node.js/Python configurado
    - [ ] Azure Tools instaladas
    - [ ] Acceso a archivos Windows

## ⚠️ Solución de Problemas

??? warning "Problemas Comunes"
    | Problema | Solución |
    |----------|----------|
    | WSL no inicia | `wsl --shutdown` y reiniciar |
    | VS Code no conecta | Reinstalar extensión WSL |
    | Permisos denegados | Usar `sudo` |
    | Actualización fallida | `wsl --update` |

## 📚 Recursos

??? info "Enlaces Útiles"
    - [Documentación oficial WSL](https://docs.microsoft.com/windows/wsl/)
    - [VS Code con WSL](https://code.visualstudio.com/docs/remote/wsl)
    - [Guía de inicio WSL](https://docs.microsoft.com/windows/wsl/setup/environment)
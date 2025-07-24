<div id="header" align="center">
    <h1>Documentación de Equipo de Desarrollo El Alto 🚀</h1>
    <h3>Aquí podrás encontrar la documentación de todas las apps disponibles: frontend, backend y guías de cómo se fueron levantando los distintos servicios.</h3>
    <img src="https://i.giphy.com/25UxpWoNBu8nOtURm9.webp" width="200"/>
</div>

<div align="center">
    <h2>📊 Arquitectura del Sistema</h2>
</div>

```mermaid
flowchart LR
Client[Cliente Web]
API[FastAPI Backend]
DB[(MySQL DB)]
SAP[SAP ERP]
Azure[Azure Cloud]
Client -->|HTTP/HTTPS| API
API -->|Queries| DB
API <-->|RFC| SAP
API -->|Deployed on| Azure
```

<div align="center">
    <h2>🔄 Flujo de Trabajo Git</h2>
</div>

```mermaid
gitGraph
commit id: "inicial"
branch qa
checkout qa
commit id: "qa-inicial"
branch develop
checkout develop
commit id: "dev-inicial"
branch feature/nueva
checkout feature/nueva
commit id: "desarrollo-feature"
commit id: "feature-completa"
checkout develop
merge feature/nueva
commit id: "dev-estable"
checkout qa
merge develop
commit id: "qa-validado"
checkout main
merge qa tag: "v1.0.0"
```


<div align="center">
    <h2>🛠️ Tecnologías Implementadas</h2>
</div>

<div align="center">
    <h3>Cloud y Backend:</h3>
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/azure/azure-original.svg" title="Azure" alt="Azure" width="40" height="40">&nbsp;
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/python/python-original.svg" title="Python" alt="Python" width="40" height="40">&nbsp;
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/fastapi/fastapi-original.svg" title="FastAPI" alt="FastAPI" width="40" height="40">&nbsp;
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/mysql/mysql-original.svg" title="SQL" alt="SQL" width="40" height="40">&nbsp;
</div>

<div align="center">
    <h3>Frontend:</h3>
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/typescript/typescript-original.svg" title="TypeScript" alt="TypeScript" width="40" height="40">&nbsp;
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/react/react-original.svg" title="React" alt="React" width="40" height="40">&nbsp;
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/nextjs/nextjs-original.svg" title="Next.js" alt="Next.js" width="40" height="40">&nbsp;
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/css3/css3-original.svg" title="CSS" alt="CSS" width="40" height="40">&nbsp;
    <img src="https://raw.githubusercontent.com/devicons/devicon/ca28c779441053191ff11710fe24a9e6c23690d6/icons/tailwindcss/tailwindcss-original.svg" title="Tailwind" alt="Tailwind" width="40" height="40">&nbsp;
</div>

<!-- <div align="center">
    <h2>⚡ Comandos Útiles</h2>
</div>

### Backend

=== "Windows"
	```bash
	func start --functions NombreApp --useHttps --cert certificate.pfx --password 'password'
	```

=== "Linux"
	```bash
	func start --functions NombreApp --useHttps
	```

### Frontend

```bash
npm run dev # Iniciar servidor de desarrollo
npm run build # Construir aplicación
npm run start # Iniciar aplicación en producción
```
<div align="center">
    <h2>🚀 Comenzando</h2>
    <p>Para comenzar a trabajar con el proyecto, dirígete a las siguientes secciones:</p>
</div>

- [📘 Guía de inicio Backend](/backend/levantamiento/windows_linux)
- [📗 Guía de inicio Frontend](/frontend/react/levantar.md)

<div align="center">
    <h2>🎩 Versiones y Análisis</h2>
    <p>Si quieres ver las versiones y análisis de las apps, dirígete a la siguiente Página:</p>
    <a href="https://desarrollo.streamlit.app/">Desarrollo El Alto</a>
</div> -->
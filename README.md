# 🤖 Bumblebee: Robot Sigue-Línea Autónomo
> **Proyecto de Desarrollo Tecnológico** · *Semillero de Investigación ERA (Electrónica, Robótica y Automatización)*

Bumblebee es un robot móvil autónomo diseñado para el seguimiento de trayectorias (línea negra sobre fondo blanco). Cuenta con un sistema de control de tracción diferencial optimizado por hardware y software, e integra una interfaz visual interactiva mediante una pantalla LCD $I^2C$ que despliega animaciones faciales en tiempo real según el estado de los sensores y la navegación.

---

## 📸 Vista del Prototipo y Diseño

| 🦾 Hardware Real | 📐 Modelo 3D (CAD) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/05a4716f-a9c3-44a6-9724-d458b6b4c935" width="340" alt="Bumblebee Prototipo Físico"> | *[Espacio reservado para el render/modelo 3D]* |

---

## 🛠️ Características Principales

* **Control Dinámico de Curvas:** Algoritmo de velocidad independiente por motor para evitar derrapes y pérdidas de línea sin comprometer el torque de arranque.
* **Interfaz de Usuario Expresiva:** Renderizado eficiente de caracteres personalizados en LCD 16x2 mediante comunicación $I^2C$, con refresco controlado no bloqueante.
* **Monitoreo Remoto:** Telemetría integrada por puerto Serial para depuración de sensores infrarrojos en tiempo real.

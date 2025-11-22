#!/bin/bash

echo "============================================="
echo "Configuracion automatica de entorno"

if command -v python3 &> /dev/null; then
    PY=python3
elif command -v python &> /dev/null; then
    PY=python
else
    echo "No se encontró Python instalado. Instala Python 3.9+."
    exit 1
fi
cd frontend
echo "Usando Python: $PY"
echo "Creando entorno virtual..."
$PY -m venv env
echo "Activando entorno..."
source env/bin/activate

echo "Instalando dependencias..."
pip install --upgrade pip
pip install flask
pip install nltk
pip install wordcloud
pip install requests
pip install matplotlib

echo "Entorno instalado correctamente"
echo "============================================="

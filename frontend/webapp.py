from flask import Flask, request, jsonify, render_template
from preprocessing import preprocess_text
from wordCloud import generar_wordcloud 
import socket
import json
import os
import glob
import logging
import time
logging.basicConfig(level=logging.DEBUG)

app = Flask(__name__)

# Internal counter for folder insertions
current_folder = 1

# Función que envía los tokens al backend C++
def send_to_backend(payload):
    HOST = "127.0.0.1"
    PORT = 8081
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(json.dumps(payload).encode())

            data = ""
            while True:
                chunk = s.recv(4096).decode()
                if not chunk:
                    break
                data += chunk
                if "<END>" in chunk:
                    break

            data = data.replace("<END>", "")
            return json.loads(data)

    except Exception as e:
        print("Error comunicando con backend:", e)
        return {"status": "error", "message": str(e)}

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/insertar', methods=['POST'])
def insert_text():
    global current_folder
    # Construct folder path
    folder_path = f"../data/ins{current_folder}"
    # Find all .txt files in the folder
    txt_files = glob.glob(os.path.join(folder_path, "*.txt"))
    if not os.path.isdir(folder_path) or len(txt_files) == 0:
        return jsonify({
            "status": "error",
            "message": f"No .txt files found in folder {folder_path}"
        }), 400
    preprocess_start = time.time()
    all_tokens = []
    for file_path in txt_files:
        try:
            try:
                # first attempt: UTF-8
                with open(file_path, "r", encoding="utf-8") as f:
                    content = f.read()
            except UnicodeDecodeError:
                try:
                    # fallback: Latin-1
                    with open(file_path, "r", encoding="latin-1") as f:
                        content = f.read()
                except Exception:
                    app.logger.warning(f"Skipping unreadable file (encoding error): {file_path}")
                    continue

            tokens = preprocess_text(content)
            all_tokens.extend(tokens)

        except Exception as e:
            app.logger.warning(f"Skipping file due to unexpected error: {file_path} ({str(e)})")
            continue
    preprocess_end = time.time()
    print(f"Tiempo de preprocesamiento: {(preprocess_end - preprocess_start) * 1000:.2f} ms")
    payload = {"action": "insertar", "tokens": all_tokens}
    backend_response = send_to_backend(payload)
    tiempo_insercion = backend_response.get("tiempo_insercion", -1)
    print(f"\nTiempo de inserción backend: {tiempo_insercion} ms")
    current_folder += 1
    return jsonify({
        "preprocessed_tokens": all_tokens,
        "backend_response": backend_response,
        "tiempo_insercion": tiempo_insercion
    })



@app.route('/consultar', methods=['POST'])
def query_backend():
    try:
        data = request.json
        k = int(data.get("k", 0))
        inicio = int(data.get("inicio", 0))
        fin = int(data.get("fin", 0))

        payload = {
            "action": "consultar",
            "k": k,
            "inicio": inicio,
            "fin": fin
        }

        backend_response = send_to_backend(payload)

        if backend_response.get("status") != "ok":
            return jsonify({
                "status": "error",
                "message": backend_response.get("message", "Error en backend")
            })

        #trending topics devueltos
        trending = backend_response.get("results", [])
        #print("DEBUG backend_response:", backend_response)
        tiempo_consulta = backend_response.get("tiempo_consulta", -1)
        #print(f"\nTiempo de consulta backend: {tiempo_consulta} ms")
        #print("DEBUG trending:", trending)
        #print("len:", len(trending))
        # generar la wordcloud
        img_path = generar_wordcloud(trending, "static/wordcloud.png")

        return jsonify({
            "status": "ok",
            "trending": trending,
            "wordcloud": img_path,
            "tiempo_consulta": tiempo_consulta
        })

    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 400

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080, debug=True)
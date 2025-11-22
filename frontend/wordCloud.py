import logging
logging.getLogger('matplotlib').setLevel(logging.WARNING)
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from wordcloud import WordCloud
import os

def generar_wordcloud(trending, output_path="static/wordcloud.png"):
    # trending = lista de dicts: [{ "topic": "algo", "frequency": 123 }, ...]
    freqs = { item["topic"]: item["frequency"] for item in trending }

    wc = WordCloud(
        width=900,
        height=1500,
        background_color="white",
        colormap="viridis"
    ).generate_from_frequencies(freqs)

    # Crear carpeta si no existe
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    plt.figure(figsize=(9, 15), dpi=200)
    plt.imshow(wc, interpolation="bilinear")
    plt.axis("off")
    plt.tight_layout()
    
    plt.savefig(output_path, dpi=200)
    plt.close()

    return output_path
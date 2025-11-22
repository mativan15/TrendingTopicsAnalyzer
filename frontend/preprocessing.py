import re
import nltk
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer

nltk.download('stopwords', quiet=True)

stop_words = set(stopwords.words('english'))
stemmer = PorterStemmer()

def preprocess_text(text):
    text = text.lower()
    text = re.sub(r'[^a-z\s]', ' ', text)
    text = re.sub(r'\s+', ' ', text).strip()
    tokens = text.split()

    tokens = [t for t in tokens if t not in stop_words]
    otras_stopwords = {"new", "news", "inc", "com", "oct","may","would","could","www","http"}
    tokens = [t for t in tokens if t not in otras_stopwords]

    tokens = [stemmer.stem(t) for t in tokens]
    tokens = [t for t in tokens if 2 < len(t) < 20 and not t.isdigit()]

    return tokens

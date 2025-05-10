import base64
from cryptography.hazmat.primitives.asymmetric import ed25519
from cryptography.hazmat.primitives import serialization

def generate_ed25519_keys():
    # 1. Генерация приватного ключа Ed25519
    private_key = ed25519.Ed25519PrivateKey.generate()
    
    # 2. Получение публичного ключа
    public_key = private_key.public_key()
    
    # 3. Сериализация ключей в PEM формат
    pem_private = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.NoEncryption()
    )
    
    pem_public = public_key.public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )
    
    # 4. Конвертация приватного ключа в base64url-safe без padding
    # Сначала получаем сырые байты приватного ключа
    raw_private = private_key.private_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PrivateFormat.Raw,
        encryption_algorithm=serialization.NoEncryption()
    )
    
    # Конвертируем в base64url-safe без padding
    b64url_private = base64.urlsafe_b64encode(raw_private).decode('utf-8').rstrip('=')
    
    return pem_private, pem_public, b64url_private

# Генерация ключей
private_pem, public_pem, private_b64url = generate_ed25519_keys()

# Вывод результатов
print("1. ED25519 Private Key (PEM):")
print(private_pem.decode('utf-8'))

print("\n2. ED25519 Public Key (PEM):")
print(public_pem.decode('utf-8'))

print("\n3. Private Key as base64url-safe no padding:")
print(private_b64url)
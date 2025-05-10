import base64
from cryptography.hazmat.primitives.asymmetric import ed25519
from cryptography.hazmat.primitives import serialization

def generate_ed25519_keys():
    # 1. Генерация приватного ключа Ed25519
    private_key = ed25519.Ed25519PrivateKey.generate()
    
    # 2. Сериализация ключей в PEM формат
    pem_private = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.NoEncryption()
    )
    
    pem_public = private_key.public_key().public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )
    
    # 3. Получаем raw байты приватного и публичного ключей
    sk = private_key.private_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PrivateFormat.Raw,
        encryption_algorithm=serialization.NoEncryption()
    )
    
    pk = private_key.public_key().public_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PublicFormat.Raw
    )
    
    # 4. Склеиваем приватный и публичный ключи (64 байта)
    full = sk + pk
    
    # 5. Конвертируем в base64url-safe без padding
    b64_full = base64.urlsafe_b64encode(full).rstrip(b'=')
    b64_pk = base64.urlsafe_b64encode(pk).rstrip(b'=')
    
    return pem_private, pem_public, b64_full.decode('utf-8'), b64_pk.decode('utf-8')

# Генерация ключей
private_pem, public_pem, b64_full, b64_pk = generate_ed25519_keys()

# Вывод результатов
print("1. ED25519 Private Key (PEM):")
print(private_pem.decode('utf-8'))

print("\n2. ED25519 Public Key (PEM):")
print(public_pem.decode('utf-8'))

print("\n3. Full key (sk+pk) as base64url-safe no padding:")
print(b64_full)

print("\n4. Public key only as base64url-safe no padding:")
print(b64_pk)
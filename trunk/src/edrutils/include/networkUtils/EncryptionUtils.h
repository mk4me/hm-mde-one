/********************************************************************
    created:  2015/12/07 19:00
    filename: EncryptionUtils.h
    author:   Wojciech Knie�
    
    purpose: 
*********************************************************************/
#ifndef HEADER_GUARD_NETWORKUTILS__EncryptionUtils_h__
#define HEADER_GUARD_NETWORKUTILS__EncryptionUtils_h__

#include <networkUtils/Export.h>
#include <QtCore/QString>
#include <array>

namespace networkUtils {
	namespace EncryptionUtils 
	{
		const int EncryptionKeyLength = 16;
		typedef std::array<unsigned char, EncryptionKeyLength + 1> EncryptionKey;
		//! Szyfruje tekst metod� aes
		//! \param txt tekst do szyfrowania
		//! \return zwraca zaszyfrowany teskt
		NETWORKUTILS_EXPORT QString encrypt(const QString& txt);
		//! Szyfruje tekst metod� aes
		//! \param txt tekst do szyfrowania
		//! \param key1 16 bajtowy klucz szyfrujacy np. {"aaaabbbbccccdddd"}
		//! \param key2 16 bajtowy klucz szyfrujacy
		//! \return zwraca zaszyfrowany teskt
		NETWORKUTILS_EXPORT QString encrypt(const QString& txt, const EncryptionKey& key1, const EncryptionKey& key2);
		//! Deszyfruje tekst metod� aes
		//! \param txt tekst do rozszyfrowania
		//! \return zwraca rozszyfrowany teskt
		NETWORKUTILS_EXPORT QString decrypt(const QString& txt);	  
		//! Deszyfruje tekst metod� aes
		//! \param txt tekst do rozszyfrowania
		//! \param key1 16 bajtowy klucz deszyfrujacy np. {"aaaabbbbccccdddd"}
		//! \param key2 16 bajtowy klucz deszyfrujacy
		//! \return zwraca zaszyfrowany teskt
		NETWORKUTILS_EXPORT QString decrypt(const QString& txt, const EncryptionKey& key1, const EncryptionKey& key2);
		//! Zamienia bufor na posta� base64
		//! \param buffer bufor do konwersji
		//! \return String w postacji base64
		NETWORKUTILS_EXPORT QString toBase64(const QByteArray& buffer);
		//! Zamienia tekst w postaci base64 na odpowiednik w postacji bufora
		//! \param buffer teskt do zamiany
		//! \return bufor powsta�y z base64
		NETWORKUTILS_EXPORT QByteArray fromBase64(const QString& buffer);
		//! Zamienia QString na typ EncryptionKey
		//! \param key Klucz do konwersji, musi mie� odpowiedni� d�ugo�� (16 znak�w)
		//! \return Klucz w postaci EncryptionKey (std::array<char, 16 + 1>)
		NETWORKUTILS_EXPORT EncryptionKey convertEncryptionKey(const QString& key);
	};
}

#endif



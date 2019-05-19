#pragma once

#include <Crypto/BigInteger.h>
#include <Crypto/ScryptParameters.h>
#include <Core/Serialization/Serializer.h>
#include <Core/Serialization/ByteBuffer.h>

static const uint8_t ENCRYPTED_SEED_FORMAT = 1;

class EncryptedSeed
{
public:
	EncryptedSeed(CBigInteger<16>&& iv, CBigInteger<8>&& salt, std::vector<unsigned char>&& encryptedSeedBytes, ScryptParameters&& scryptParameters)
		: m_iv(std::move(iv)), m_salt(std::move(salt)), m_encryptedSeedBytes(std::move(encryptedSeedBytes)), m_scryptParameters(std::move(scryptParameters))
	{

	}

	inline const CBigInteger<16>& GetIV() const { return m_iv; }
	inline const CBigInteger<8>& GetSalt() const { return m_salt; }
	inline const std::vector<unsigned char>& GetEncryptedSeedBytes() const { return m_encryptedSeedBytes; }
	inline const ScryptParameters& GetScryptParameters() const { return m_scryptParameters; }

	void Serialize(Serializer& serializer) const
	{
		serializer.Append<uint8_t>(ENCRYPTED_SEED_FORMAT);
		serializer.AppendBigInteger<16>(m_iv);
		serializer.AppendBigInteger<8>(m_salt);
		serializer.Append<uint32_t>(m_scryptParameters.N);
		serializer.Append<uint32_t>(m_scryptParameters.r);
		serializer.Append<uint32_t>(m_scryptParameters.p);
		serializer.AppendByteVector(m_encryptedSeedBytes);
	}

	static EncryptedSeed Deserialize(ByteBuffer& byteBuffer)
	{
		const uint8_t version = byteBuffer.ReadU8();
		if (version > ENCRYPTED_SEED_FORMAT)
		{
			throw DeserializationException();
		}

		CBigInteger<16> iv = byteBuffer.ReadBigInteger<16>();
		CBigInteger<8> salt = byteBuffer.ReadBigInteger<8>();

		ScryptParameters parameters(131072, 16, 2);
		if (version >= 1)
		{
			const uint32_t N = byteBuffer.ReadU32();
			const uint32_t r = byteBuffer.ReadU32();
			const uint32_t p = byteBuffer.ReadU32();
			parameters = ScryptParameters(N, r, p);
		}

		std::vector<unsigned char> encryptedSeedBytes = byteBuffer.ReadVector(byteBuffer.GetRemainingSize());

		return EncryptedSeed(std::move(iv), std::move(salt), std::move(encryptedSeedBytes), std::move(parameters));
	}

private:
	CBigInteger<16> m_iv;
	CBigInteger<8> m_salt;
	std::vector<unsigned char> m_encryptedSeedBytes;
	ScryptParameters m_scryptParameters;
};
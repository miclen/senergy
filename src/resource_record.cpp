/*******************************************************************************
 *
 *	 This file is part of Senergy.
 *
 *   Senergy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Senergy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Senergy. If not, see <http://www.gnu.org/licenses/>.
 *
 *   Swen Kooij (Photonios) <swenkooij@gmail.com> <photonios@outlook.com>
 *
 *******************************************************************************/

#include <senergy/dns/resource_record.h>

namespace Senergy
{
namespace Dns
{

ResourceRecord::ResourceRecord() :
	m_hostname	(""),
	m_type		(ResourceRecordType::A),
	m_class		(ResourceRecordClass::Internet),
	m_ttl		(0),
	m_rd_length	(0)
{
}

bool ResourceRecord::Deserialize(ByteBuffer &buffer)
{
	if(buffer.GetRemainingSize() < 4)
		return false;

	m_hostname = buffer.ReadString(256);

	if(m_hostname.size() < 2)
		return false;

	// we do not support pointers..
	if(Utils::IsDomainPointer(m_hostname))
		return false;

	m_hostname = Utils::DecodeHostname(m_hostname);

	if(buffer.GetRemainingSize() < 4)
		return false;

	m_type 		= (ResourceRecordType) buffer.ReadUnsignedShort();
	m_class 	= (ResourceRecordClass) buffer.ReadUnsignedShort();
	m_ttl 		= buffer.ReadUnsignedInt();
	m_rd_length = buffer.ReadUnsignedShort();

	m_type 		= (ResourceRecordType) Utils::NetworkToHostByteOrder((unsigned short)m_type);
	m_class 	= (ResourceRecordClass) Utils::NetworkToHostByteOrder((unsigned short)m_class);
	m_ttl 		= Utils::NetworkToHostByteOrder(m_ttl);
	m_rd_length = Utils::NetworkToHostByteOrder(m_rd_length);
	return true;
}

bool ResourceRecord::Serialize(ByteBuffer &buffer)
{
	if(m_hostname.empty() || m_rd_length <= 0)
		return false;

	std::string encoded_hostname = Utils::EncodeHostname(m_hostname);
	buffer.Write(encoded_hostname);

	buffer.Write(Utils::HostToNetworkByteOrder((unsigned short)m_type));
	buffer.Write(Utils::HostToNetworkByteOrder((unsigned short)m_class));
	buffer.Write(Utils::HostToNetworkByteOrder(m_ttl));
	buffer.Write(Utils::HostToNetworkByteOrder(m_rd_length));
	return true;
}

std::string ResourceRecord::GetHostname()
{
	return m_hostname;
}

void ResourceRecord::SetHostname(const std::string hostname)
{
	m_hostname = hostname;
}

ResourceRecordType ResourceRecord::GetType()
{
	return m_type;
}

void ResourceRecord::SetType(ResourceRecordType type)
{
	m_type = type;
}

ResourceRecordClass ResourceRecord::GetClass()
{
	return m_class;
}

void ResourceRecord::SetClass(ResourceRecordClass clas)
{
	m_class = clas;
}

bool ResourceRecord::NeedsCaching()
{
	return (m_ttl > 0);
}

int ResourceRecord::GetMaxCachingTime()
{
	return (int)m_ttl;
}

int ResourceRecord::GetResourceSize()
{
	return (int)m_rd_length;
}

} // namespace Dns
} // namespace Senergy

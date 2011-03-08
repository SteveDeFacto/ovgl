/**
* @file NxStreamDefault.h
* Copyright 2011 Steven Batchelor
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* @brief None.
*/

#include "NxPhysics.h"
#include "NxStream.h"

class NxStreamDefault : public NxStream
{
public:

	mutable	NxU32			loc;
	NxU32					currentSize;
	NxU32					maxSize;
	NxU8*					data;

	NxStreamDefault() : loc(0), currentSize(0), maxSize(0), data(NULL)
	{
	}

	~NxStreamDefault()
	{
		NX_DELETE_ARRAY(data);
	}

	void clear()
	{
		currentSize = 0;
	}

	NxU8 readByte() const
	{
		NxU8 b;
		memcpy(&b, data + loc, sizeof(NxU8));
		loc += sizeof(NxU8);
		return b;
	}

	NxU16 readWord() const
	{
		NxU16 w;
		memcpy(&w, data + loc, sizeof(NxU16));
		loc += sizeof(NxU16);
		return w;
	}

	NxU32 readDword() const
	{
		NxU32 d;
		memcpy(&d, data + loc, sizeof(NxU32));
		loc += sizeof(NxU32);
		return d;
	}

	float readFloat() const
	{
		float f;
		memcpy(&f, data + loc, sizeof(float));
		loc += sizeof(float);
		return f;
	}

	double readDouble() const
	{
		double f;
		memcpy(&f, data + loc, sizeof(double));
		loc += sizeof(double);
		return f;
	}

	void readBuffer(void* dest, NxU32 size) const
	{
		memcpy(dest, data + loc, size);
		loc += size;
	}


	NxStream& storeByte(NxU8 b)
	{
		storeBuffer(&b, sizeof(NxU8));
		return *this;
	}

	NxStream& storeWord(NxU16 w)
	{
		storeBuffer(&w, sizeof(NxU16));
		return *this;
	}

	NxStream& storeDword(NxU32 d)
	{
		storeBuffer(&d, sizeof(NxU32));
		return *this;
	}

	NxStream& storeFloat(NxReal f)
	{
		storeBuffer(&f, sizeof(NxReal));
		return *this;
	}

	NxStream& storeDouble(NxF64 f)
	{
		storeBuffer(&f, sizeof(NxF64));
		return *this;
	}
	NxStream& storeBuffer(const void* buffer, NxU32 size)
	{
		NxU32 expectedSize = currentSize + size;
		if(expectedSize > maxSize)
		{
			maxSize = expectedSize + 4096;
			NxU8* newData = new NxU8[maxSize];
			NX_ASSERT(newData!=NULL);
			if(data)
			{
				memcpy(newData, data, currentSize);
				delete[] data;
			}
				data = newData;
		}
		memcpy(data+currentSize, buffer, size);
		currentSize += size;
		return *this;
	}
};
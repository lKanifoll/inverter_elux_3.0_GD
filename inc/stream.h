#ifndef heads_stream_h
#define heads_stream_h

#include "stdio.h"
#include "string.h"
		
class Stream
{
  private:

    uint8_t* m_sptr; // start
		uint8_t* m_cptr; // current
		uint8_t* m_eptr; // end

  public:
    Stream(uint8_t *data_buffer, size_t data_len)
      : m_sptr(data_buffer), m_cptr(data_buffer), m_eptr(data_buffer + data_len)
    {
    }

		uint8_t* sptr() { return m_sptr; };

		size_t readBytes(uint8_t *buffer, size_t length)
		{
			size_t count = 0;
			while (count < length && good()) 
			{
				*buffer++ = get();
				count++;
			}
			return count;
		};
		

		int good()
		{
			return (m_cptr == m_eptr) ? 0 : 1;
		}

    uint8_t get() 
		{
			if (good()) 
				return *m_cptr++;
			
			return 0;
    }

		
    size_t count()
		{
      return m_cptr - m_sptr;
    };

    Stream& put(uint8_t d) 
		{
			if (good())
				*m_cptr++ = d;
			
      return *this;
    };
		
    Stream& put_str(uint8_t* data, uint8_t len) 
		{
			uint8_t *temp_data = data;
			if (good())
			{
				while(len--)
				{
					*m_cptr++ = *temp_data++;
				}
			}
      return *this;
    };
		
    int peek() 
		{
      return *(m_cptr);
    }

    virtual void flush() 
		{
			memset(m_sptr, 0, m_eptr - m_sptr);
      m_cptr = m_sptr;
    }

    Stream& clear() {
      this->flush();
      return *this;
    }

    Stream& reset() 
		{
      m_cptr = m_sptr;
      return *this;
    }

};
#endif

#include <stdlib.h>
#include <string.h>

struct CC4Policy_tag;
class CC4Policies;

typedef struct CC4Policy_tag
{
	unsigned char m_begin;
	unsigned char m_end;
	bool          m_readnext;

	CC4Policy_tag()
	{
		m_begin = 0x00;
		m_end   = 0x7F;
		m_readnext = false;
	};
}CC4Policy, *PCC4Policy;

class CC4Policies
{
private:
	CC4Policy*    m_policies;
	unsigned int  m_size;
public:
	CC4Policies(unsigned int num_policies);
	~CC4Policies();
	bool checkValid() const;
	bool setPolicy(unsigned int position, const CC4Policy& policy);
	bool isContinueReadNextChar(unsigned char chr) const;
};
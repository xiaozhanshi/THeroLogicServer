#ifndef  __NON_COPYABLE_CLASS_H__
#define  __NON_COPYABLE_CLASS_H__


// NonCopyableClass.h


class  NonCopyableClass
{
private:
	NonCopyableClass( const NonCopyableClass & );
	const NonCopyableClass & operator=( const NonCopyableClass & );

public:
	NonCopyableClass(){};
	virtual ~NonCopyableClass(){};
};

#endif // __NON_COPYABLE_CLASS_H__

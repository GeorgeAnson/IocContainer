#ifndef NON_COPYABLE_H_H__H
#define NON_COPYABLE_H_H_H
//创建不可复制类
class NonCopyable
{
protected:
	NonCopyable() = default;
	~NonCopyable() = default;
	//禁止复制构造
	NonCopyable(const NonCopyable&) = delete;
	//禁止复制构造
	NonCopyable& operator=(const NonCopyable&) = delete;
};
#endif
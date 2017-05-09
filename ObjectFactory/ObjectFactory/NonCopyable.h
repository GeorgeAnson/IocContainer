#ifndef NON_COPYABLE_H_H__H
#define NON_COPYABLE_H_H_H
//�������ɸ�����
class NonCopyable
{
protected:
	NonCopyable() = default;
	~NonCopyable() = default;
	//��ֹ���ƹ���
	NonCopyable(const NonCopyable&) = delete;
	//��ֹ���ƹ���
	NonCopyable& operator=(const NonCopyable&) = delete;
};
#endif
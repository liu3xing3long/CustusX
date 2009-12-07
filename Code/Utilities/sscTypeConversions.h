#ifndef SSCTYPECONVERSIONS_H_
#define SSCTYPECONVERSIONS_H_

#include <string>
#include <QString>
#include <QVariant>
#include <boost/lexical_cast.hpp>

/**Convenience function that converts a type to its 
 * string representation, provided it has defined operator <<.
 */
template<class T>
std::string string_cast(const T& val)
{
	return boost::lexical_cast<std::string>(val);
}

/**Convenience function that converts a type to its 
 * QString representation, provided it has defined operator <<.
 */
template<class T> QString qstring_cast(const T& val)
{
	return QString::fromStdString(string_cast(val));
}

/**utitity class for the cstring_cast function
 */
class cstring_cast_Placeholder
{
public:
	explicit cstring_cast_Placeholder(const std::string& val) : mData(val) {}
	operator const char*() const { return mData.c_str(); }
	const char* c() const { return mData.c_str(); }
private:
	std::string mData;
};

/**Convenience function that converts a type to its 
 * char* representation, provided it has defined operator <<
 * or is QString or QVariant
 */
template<class T> cstring_cast_Placeholder cstring_cast(const T& val)
{
	return cstring_cast_Placeholder(string_cast(val));
}
template<> cstring_cast_Placeholder cstring_cast<QString>(const QString& val);
template<> cstring_cast_Placeholder cstring_cast<QVariant>(const QVariant& val);

/** Helper function overload for streaming a QString to std::cout.
 */
std::ostream& operator<<(std::ostream& str, const QString& qstring);

#endif /*SSCTYPECONVERSIONS_H_*/

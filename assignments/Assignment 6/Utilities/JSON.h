#ifndef MiniData_JSON_Included
#define MiniData_JSON_Included

#include <memory>
#include <istream>
#include <ostream>
#include <vector>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace MiniData_JSONImpl {
  struct NullTag    {};
  struct BoolTag    {};
  struct IntegerTag {};
  struct DoubleTag  {};
  struct StringTag  {};
  struct ArrayTag   {};
  struct MapTag     {};

  template <typename T> struct IsJSONConvertible;
  template <typename T> struct IsString;
  
  class BaseJSON;
  class JSONSource;
}

/* Type representing a value represented in JSON format. */
class JSON {
public:
    /* Wraps the specified value as a JSON object. This constructor can accept
     * as inputs any of the following types:
     *
     *   nullptr_t
     *   bool
     *   any integral type
     *   any floating-point type
     *   raw C strings
     *   std::string
     *   arrays
     *   any sequence container of items convertible to JSONs
     *   any associative container whose key type can be converted to string and whose value type can be converted to JSON
     */
    template <
      typename T,
      typename = typename std::enable_if<MiniData_JSONImpl::IsJSONConvertible<T>::value>::type
    > JSON(const T& value);

    /* Parses a piece of text into JSON format. The expectation is that the input
     * consists of the full JSON and nothing else. To read just one JSON object
     * from a stream, use operator>>.
     */
    inline static JSON parse(std::istream& input);
    inline static JSON parse(const std::string& input);

    /* Builds an array or object. */
    inline static JSON array (std::initializer_list<JSON> elems = {});
    template <typename... T> static JSON array(const T&... args);
    
    inline static JSON object(std::initializer_list<std::pair<const std::string, JSON>> elems = {});

    /* Enumeration representing what type of object this is. */
    enum class Type {
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN,
        NULLPTR_T
    };

    /* Returns the type of this object. */
    inline Type type() const;

    /* Accessors. All of these functions will throw a JSONException if the underlying type
     * is incorrect.
     */
    inline double         asDouble()  const;
    inline std::int64_t   asInteger() const;
    inline bool           asBoolean() const;
    inline std::nullptr_t asNull()    const;
    inline std::string    asString()  const;

    /* Array accessors. Again, these will throw JSONExceptions if the underlying type is
     * incorrect.
     */
    inline JSON operator[] (std::size_t index) const;

    /* Object accessors. As usual, these throw JSONExceptions if the underlying type is
     * incorrect.
     */
    inline JSON operator[] (const std::string& field) const;

    /* Something convertible to string. */
    template <
      typename String,
      typename = typename std::enable_if<MiniData_JSONImpl::IsString<String>::value>::type
    >
    JSON operator[] (const String& field) const {
      return (*this)[std::string(field)];
    }
    inline bool contains(const std::string& fieldName) const;
    
    /* Generic accessor. If the wrapped JSON object is a number, looks up the element at that index,
     * assuming this is an array. If the wrapped JSON object is a string, looks up the element with
     * that key, assuming this is an object.
     *
     * Be careful calling this function with wrapped doubles; any rounding errors will cause
     * problems!
     */
    inline JSON operator[] (JSON key) const;

    /* Shared between arrays and objects. */
    inline std::size_t size() const;

    /* For arrays, iterates over the elements. For objects, iterates over the keys as JSON strings.
     * For all other types, raises an error.
     */
    class const_iterator;
    inline const_iterator begin()  const;
    inline const_iterator end()    const;
    inline const_iterator cbegin() const;
    inline const_iterator cend()   const;
    
    inline friend std::ostream& operator<< (std::ostream& out, JSON json);
    inline friend std::istream& operator>> (std::istream& in,  JSON& json);

private:
    friend class MiniData_JSONImpl::BaseJSON;
    std::shared_ptr<MiniData_JSONImpl::BaseJSON> mImpl;

    struct ConstructorTag {};
    inline JSON(std::shared_ptr<MiniData_JSONImpl::BaseJSON> impl, ConstructorTag);

    /* Construct JSON objects from objects of the appropriate types. */
    inline static std::shared_ptr<MiniData_JSONImpl::BaseJSON> fromNull(std::nullptr_t value);
    inline static std::shared_ptr<MiniData_JSONImpl::BaseJSON> fromDouble(double value);
    inline static std::shared_ptr<MiniData_JSONImpl::BaseJSON> fromInteger(std::int64_t value);
    inline static std::shared_ptr<MiniData_JSONImpl::BaseJSON> fromBoolean(bool value);
    inline static std::shared_ptr<MiniData_JSONImpl::BaseJSON> fromString(const std::string& value);

    /* Constructs a JSON array from a list of JSON objects. */
    inline static std::shared_ptr<MiniData_JSONImpl::BaseJSON> fromArray(const std::vector<JSON>& elems);

    /* Constructs a JSON object from a map. */
    inline static std::shared_ptr<MiniData_JSONImpl::BaseJSON> fromMap(const std::map<std::string, JSON>& elems);

    /* Dispatching constructors. */
    template <typename T> JSON(MiniData_JSONImpl::NullTag,    const T& value) : mImpl(fromNull(value)) {}
    template <typename T> JSON(MiniData_JSONImpl::BoolTag,    const T& value) : mImpl(fromBoolean(value)) {}
    template <typename T> JSON(MiniData_JSONImpl::IntegerTag, const T& value) : mImpl(fromInteger(value)) {}
    template <typename T> JSON(MiniData_JSONImpl::DoubleTag,  const T& value) : mImpl(fromDouble(value)) {}
    template <typename T> JSON(MiniData_JSONImpl::StringTag,  const T& value) : mImpl(fromString(value)) {}
    template <typename T> JSON(MiniData_JSONImpl::ArrayTag,   const T& value) : 
      mImpl(fromArray(std::vector<JSON>(std::begin(value), std::end(value)))) {}
    template <typename T> JSON(MiniData_JSONImpl::MapTag,     const T& value) : 
      mImpl(fromMap(std::map<std::string, JSON>(std::begin(value), std::end(value)))) {}
};

class JSON::const_iterator {
public:
    /* Iterator typedefs. */
    typedef std::ptrdiff_t          difference_type;
    typedef const JSON              value_type;
    typedef const JSON*             pointer;
    typedef const JSON&             reference;
    typedef std::input_iterator_tag iterator_category;

    inline const_iterator();
    
    inline bool operator== (const_iterator rhs) const;
    inline bool operator!= (const_iterator rhs) const;
    
    inline const_iterator& operator++ ();
    inline const const_iterator operator++ (int);
    
    inline const JSON& operator* () const;
    inline const JSON* operator->() const;

private:
    friend class MiniData_JSONImpl::JSONSource;
    std::shared_ptr<MiniData_JSONImpl::JSONSource> mImpl;
    
    inline const_iterator(std::shared_ptr<MiniData_JSONImpl::JSONSource>);
};

/* Exception type thrown when an error occurs. */
class JSONException: public std::logic_error {
public:
    inline JSONException(const std::string& reason);
};

/***** Private Implementation Details *****/
namespace MiniData_JSONImpl {
  /* Traits types to determine the best type to use for something. */

  /* Is this a null pointer? */
  template <typename T> struct IsNull {
    static const bool value = std::is_same<typename std::remove_cv<T>::type, std::nullptr_t>::value;
  };

  /* Is this a boolean value? */
  template <typename T> struct IsBool {
    static const bool value = std::is_same<typename std::remove_cv<T>::type, bool>::value;
  };

  /* Is this an integer? */
  template <typename T> struct IsInteger {
    /* Need to explicitly define bool away. */
    static const bool value = !IsBool<T>::value && std::is_integral<T>::value;
  };

  /* Is this a double? */
  template <typename T> struct IsDouble {
    static const bool value = std::is_floating_point<T>::value;
  };

  /* Is this a C-style string? */
  template <typename T> struct IsString {
    static const bool value = std::is_convertible<T, std::string>::value;
  };
  /* Is this an array of JSONs? */
  template <typename T> struct IsArray {
    /* Does std::begin give us something we can make a JSON from? */
    template <typename U> static std::true_type  evaluate(int, typename std::enable_if<std::is_convertible<typename U::value_type, JSON>::value>::type* = nullptr);
    template <typename U> static std::false_type evaluate(...);

    static const bool value = std::is_same<decltype(evaluate<T>(0)), std::true_type>::value;
  }; 
  template <size_t N> struct IsArray<JSON [N]> {
    static const bool value = true;
  };
  template <size_t N> struct IsArray<const JSON [N]> {
    static const bool value = true;
  };

  /* Is this a map of JSONs? */
  template <typename T> struct IsMap {
    /* Does std::begin give us something where .first can be made a string and
     * .second can be made a JSON?
     */
    template <typename U> using value_type = typename U::value_type;
    template <typename U> using k_type   = decltype(std::declval<value_type<U>>().first);
    template <typename U> using v_type = decltype(std::declval<value_type<U>>().second);
    template <typename U> static std::true_type  evaluate(int,
                                                          typename std::enable_if<IsString<k_type<U>>::value>* = nullptr,
                                                          typename std::enable_if<std::is_convertible<v_type<U>, JSON>::value>::type* = nullptr);
    template <typename U> static std::false_type evaluate(...);

    static const bool value = std::is_same<decltype(evaluate<T>(0)), std::true_type>::value;
  };

  template <typename T> struct TagFor {
    using type = typename std::conditional<IsNull<T>::value, NullTag,
                  typename std::conditional<IsBool<T>::value, BoolTag,
                    typename std::conditional<IsInteger<T>::value, IntegerTag,
                      typename std::conditional<IsDouble<T>::value, DoubleTag,
                        typename std::conditional<IsString<T>::value, StringTag,
                          typename std::conditional<IsArray<T>::value, ArrayTag,
                            typename std::conditional<IsMap<T>::value, MapTag, void>::type
                          >::type
                        >::type
                      >::type
                    >::type
                  >::type
                >::type;
  };

  template <typename T> struct IsJSONConvertible {
    static const bool value = !std::is_same<typename TagFor<T>::type, void>::value;
  };
}

/* Forward things to the right constructor. */
template <typename T, typename> JSON::JSON(const T& value) : JSON(typename MiniData_JSONImpl::TagFor<T>::type(), value) {

}

/* Variadic functions convert to initializer lists. */
template <typename... T> JSON JSON::array(const T&... args) {
  return array({ args... });
}

#include "Unicode.h"
#include <map>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <iomanip>
#include <string>
#include <iterator>

/* Utility function to report an error. */
namespace MiniData_JSONImpl {
    [[ noreturn ]] inline void jsonError(const std::string& message) {
        throw JSONException(message);
    }
    
    /* Base internal type for JSON objects. */
    class BaseJSON {
    public:
        virtual ~BaseJSON() = default;

        /* Returns the type of this object. */
        inline JSON::Type type() const;

        /* Outputs this object to a stream. */
        virtual void print(std::ostream& out) const = 0;

        /* Backdoor route for constructing a JSON directly from a BaseJSON. This is used
         * because BaseJSON is a friend of JSON and therefore can call its internal
         * constructor.
         */
        template <typename Type, typename... Args>
        static JSON make(Args&&... args) {
            return JSON(std::make_shared<Type>(std::forward<Args>(args)...), JSON::ConstructorTag());
        }

    protected:
        inline BaseJSON(JSON::Type type);

    private:
        JSON::Type mType;
    };

    /* Iterator support. This generator type is used to produce elements as a stream. */
    class JSONSource {
    public:
        virtual ~JSONSource() = default;
        
        virtual void advance() = 0;
        virtual bool finished() const = 0;
        
        virtual const JSON& current() const = 0;
        
        /* Constructs a const_iterator from a shared_ptr. */
        static JSON::const_iterator make(std::shared_ptr<JSONSource> impl) {
            return impl;
        }
    };

    /* Type representing null. */
    class NullJSON: public BaseJSON {
    public:
        inline NullJSON(std::nullptr_t value);
        inline std::nullptr_t value() const;
        
        inline void print(std::ostream& out) const override;
    };

    /* Type representing a boolean. */
    class BoolJSON: public BaseJSON {
    public:
        inline BoolJSON(bool value);
        inline bool value() const;
        
        inline void print(std::ostream& out) const override;

    private:
        bool mValue;
    };

    /* Type representing some form of numeric quantity. */
    class NumberJSON: public BaseJSON {
    public:
        inline NumberJSON();

        virtual double       asDouble() const  = 0;
        virtual std::int64_t asInteger() const = 0;
    };

    /* Specializations of numbers for integers and doubles. */
    template <typename NumericType> class NumericValueJSON: public NumberJSON {
    public:
        NumericValueJSON(NumericType value);
        
        inline double       asDouble()  const override;
        inline std::int64_t asInteger() const override;

        inline void print(std::ostream& out) const override;
        
    private:
        NumericType mValue;
    };

    /* Type representing a string. */
    class StringJSON: public BaseJSON {
    public:
        inline StringJSON(const std::string& value);
        inline std::string value() const;
        
        inline void print(std::ostream& out) const override;

    private:
        std::string mValue;
    };

    /* Intermediate type representing something that can be iterated over. */
    class IterableJSON: public BaseJSON {
    public:
        inline IterableJSON(JSON::Type type);

        virtual std::size_t size() const = 0;
        virtual std::shared_ptr<JSONSource> source() const = 0;
    };

    /* Type representing an array. */
    class ArrayJSON: public IterableJSON {
    public:
        inline ArrayJSON(const std::vector<JSON>& elems);

        inline std::size_t size() const override;
        inline JSON operator[] (std::size_t index) const;
        
        inline void print(std::ostream& out) const override;
        inline std::shared_ptr<JSONSource> source() const override;

    private:
        std::vector<JSON> mElems;
    };

    /* Type representing an object. */
    class ObjectJSON: public IterableJSON {
    public:
        inline ObjectJSON(const std::map<std::string, JSON>& elems);

        inline bool contains(const std::string& key) const;
        inline JSON operator[] (const std::string& key) const;
        inline std::size_t size() const override;
        
        inline void print(std::ostream& out) const override;
        inline std::shared_ptr<JSONSource> source() const override;

    private:
        std::map<std::string, JSON> mElems;
    };

/***************************************************************************/
/***********        Implementation of individual subtypes        ***********/
/***************************************************************************/

    /* Utility printing routine to output a string. */
    inline void printString(std::ostream& out, const std::string& str) {
        out << '"';
    
        std::istringstream extractor(str);
        while (extractor.peek() != EOF) {
            char32_t ch = readChar(extractor);
            
            /* See how we need to encode this character. */
            if      (ch == '"')  out << "\\\"";
            else if (ch == '\\') out << "\\\\";
            else if (ch == '/')  out << "\\/";
            else if (ch == '\b') out << "\\b";
            else if (ch == '\n') out << "\\n";
            else if (ch == '\r') out << "\\r";
            else if (ch == '\t') out << "\\t";
            else if (ch >= 0x20 && ch <= 0x7F) out << char(ch);
            else out << utf16EscapeFor(ch);
        }
        
        out << '"';
    }

    inline BaseJSON::BaseJSON(JSON::Type type) : mType(type) {

    }
    inline JSON::Type BaseJSON::type() const {
        return mType;
    }

    inline NullJSON::NullJSON(std::nullptr_t) : BaseJSON(JSON::Type::NULLPTR_T) {

    }

    inline std::nullptr_t NullJSON::value() const {
        return nullptr;
    }

    inline void NullJSON::print(std::ostream& out) const {
        out << "null";
    }

    inline StringJSON::StringJSON(const std::string& value) : BaseJSON(JSON::Type::STRING), mValue(value) {

    }

    inline std::string StringJSON::value() const {
        return mValue;
    }

    inline void StringJSON::print(std::ostream& out) const {
        printString(out, mValue);
    }

    inline NumberJSON::NumberJSON() : BaseJSON(JSON::Type::NUMBER) {

    }

    template <typename NumericType>
    NumericValueJSON<NumericType>::NumericValueJSON(NumericType value) : mValue(value) {

    }

    template <typename NumericType>
    double NumericValueJSON<NumericType>::asDouble() const {
        return static_cast<double>(mValue);
    }

    template <typename NumericType>
    int64_t NumericValueJSON<NumericType>::asInteger() const {
        return static_cast<int64_t>(mValue);
    }

    template <typename NumericType>
    void NumericValueJSON<NumericType>::print(std::ostream& out) const {
        out << mValue;
    }

    inline BoolJSON::BoolJSON(bool value) : BaseJSON(JSON::Type::BOOLEAN), mValue(value) {

    }

    inline bool BoolJSON::value() const {
        return mValue;
    }

    inline void BoolJSON::print(std::ostream& out) const {
        out << (mValue? "true" : "false");
    }

    inline IterableJSON::IterableJSON(JSON::Type type) : BaseJSON(type) {

    }

    inline ArrayJSON::ArrayJSON(const std::vector<JSON>& elems) : IterableJSON(JSON::Type::ARRAY), mElems(elems) {

    }

    inline std::size_t ArrayJSON::size() const {
        return mElems.size();
    }

    inline std::shared_ptr<JSONSource> ArrayJSON::source() const {
        /* Source just wraps a pair of iterators. */
        class VectorJSONSource: public JSONSource {
        public:
            VectorJSONSource(std::vector<JSON>::const_iterator curr, std::vector<JSON>::const_iterator end)
              : mCurr(curr), mEnd(end) {
              
            }
            
            void advance() override {
                ++mCurr;
            }
            bool finished() const override {
                return mCurr == mEnd;
            }
        
            const JSON& current() const override {
                return *mCurr;
            }
        
        private:
            std::vector<JSON>::const_iterator mCurr, mEnd;
        };
        
        return std::make_shared<VectorJSONSource>(mElems.begin(), mElems.end());
    }

    inline JSON ArrayJSON::operator[] (std::size_t index) const {
        if (index >= mElems.size()) {
            jsonError("Index out of range: " + std::to_string(index) + ", but size is " + std::to_string(size()));
        }
        return mElems[index];
    }

    inline void ArrayJSON::print(std::ostream& out) const {
        out << '[';
        for (size_t i = 0; i < mElems.size(); i++) {
            out << mElems[i] << (i + 1 == mElems.size()? "" : ",");
        }
        out << ']';
    }

    inline ObjectJSON::ObjectJSON(const std::map<std::string, JSON>& elems) : IterableJSON(JSON::Type::OBJECT), mElems(elems) {

    }

    inline bool ObjectJSON::contains(const std::string& key) const {
        return mElems.count(key);
    }

    inline JSON ObjectJSON::operator[](const std::string& key) const {
        if (!contains(key)) {
            jsonError("Key " + key + " does not exist.");
        }
        return mElems.at(key);
    }

    inline std::size_t ObjectJSON::size() const {
        return mElems.size();
    }

    inline void ObjectJSON::print(std::ostream& out) const {
        out << '{';
        for (auto itr = mElems.begin(); itr != mElems.end(); ++itr) {
            printString(out, itr->first);
            out << ":" << itr->second;
            if (next(itr) != mElems.end()) {
                out << ",";
            }
        }
        out << '}';
    }

    inline std::shared_ptr<JSONSource> ObjectJSON::source() const {
        /* Source wraps a pair of iterators and stores a JSON object representing the
         * current string.
         */
        class MapJSONSource: public JSONSource {
        public:
            MapJSONSource(std::map<std::string, JSON>::const_iterator curr,
                          std::map<std::string, JSON>::const_iterator end)
              : mCurr(curr), mEnd(end), mStaged(nullptr) {
                if (mCurr != mEnd) {
                    mStaged = JSON(mCurr->first);
                }
            }
            
            void advance() override {
                ++mCurr;
                if (mCurr != mEnd) {
                    mStaged = JSON(mCurr->first);
                }
            }
            bool finished() const override {
                return mCurr == mEnd;
            }
        
            const JSON& current() const override {
                return mStaged;
            }
        
        private:
            std::map<std::string, JSON>::const_iterator mCurr, mEnd;
            JSON mStaged;
        };
        
        return std::make_shared<MapJSONSource>(mElems.begin(), mElems.end());
    }

/***************************************************************************/
/***********          Implementation of JSON accessors           ***********/
/***************************************************************************/

    /* Safely downcasts the underlying pointer type. */
    template <typename Target> std::shared_ptr<Target> as(std::shared_ptr<BaseJSON> base) {
        auto result = std::dynamic_pointer_cast<Target>(base);
        if (!result) {
            std::ostringstream result;
            auto& baseObj = *base.get(); // Suppress Mac compiler warning on typeid
            (void) baseObj;              // Suppress Linux compiler warning
            result << "Wrong JSON type. Actual type is " << typeid(baseObj).name()
                   << ", which can't be converted to " << typeid(Target).name();
            jsonError(result.str());
        }
        return result;
    }
}

inline JSON::JSON(std::shared_ptr<MiniData_JSONImpl::BaseJSON> impl, ConstructorTag) : mImpl(impl) {

}
inline JSON::Type JSON::type() const {
    return mImpl->type();
}
inline std::nullptr_t JSON::asNull() const {
    return MiniData_JSONImpl::as<MiniData_JSONImpl::NullJSON>(mImpl)->value();
}
inline bool JSON::asBoolean() const {
    return MiniData_JSONImpl::as<MiniData_JSONImpl::BoolJSON>(mImpl)->value();
}
inline double JSON::asDouble() const {
    return MiniData_JSONImpl::as<MiniData_JSONImpl::NumberJSON>(mImpl)->asDouble();
}
inline std::int64_t JSON::asInteger() const {
    return MiniData_JSONImpl::as<MiniData_JSONImpl::NumberJSON>(mImpl)->asInteger();
}
inline std::string JSON::asString() const {
    return MiniData_JSONImpl::as<MiniData_JSONImpl::StringJSON>(mImpl)->value();
}
inline JSON JSON::operator [](std::size_t index) const {
    return (*MiniData_JSONImpl::as<MiniData_JSONImpl::ArrayJSON>(mImpl))[index];
}
inline std::size_t JSON::size() const {
    return MiniData_JSONImpl::as<MiniData_JSONImpl::IterableJSON>(mImpl)->size();
}
inline JSON JSON::operator [](const std::string& key) const {
    return (*MiniData_JSONImpl::as<MiniData_JSONImpl::ObjectJSON>(mImpl))[key];
}
inline bool JSON::contains(const std::string& key) const {
    return MiniData_JSONImpl::as<MiniData_JSONImpl::ObjectJSON>(mImpl)->contains(key);
}

inline JSON JSON::operator [](JSON key) const {
    /* Forward as appropriate. */
    if (key.type() == JSON::Type::NUMBER) return (*this)[key.asInteger()];
    if (key.type() == JSON::Type::STRING) return (*this)[key.asString()];
    
    MiniData_JSONImpl::jsonError("Cannot use this JSON object as a key.");
}

inline std::ostream& operator<< (std::ostream& out, JSON json) {
    json.mImpl->print(out);
    return out;
}

/***************************************************************************/
/***********     Implementation of JSON constructor helpers      ***********/
/***************************************************************************/
inline std::shared_ptr<MiniData_JSONImpl::BaseJSON> JSON::fromNull(std::nullptr_t) {
  return std::make_shared<MiniData_JSONImpl::NullJSON>(nullptr);
}
inline std::shared_ptr<MiniData_JSONImpl::BaseJSON> JSON::fromBoolean(bool value) {
  return std::make_shared<MiniData_JSONImpl::BoolJSON>(value);
}
inline std::shared_ptr<MiniData_JSONImpl::BaseJSON> JSON::fromDouble(double value) {
  return std::make_shared<MiniData_JSONImpl::NumericValueJSON<double>>(value);
}
inline std::shared_ptr<MiniData_JSONImpl::BaseJSON> JSON::fromInteger(std::int64_t value) {
  return std::make_shared<MiniData_JSONImpl::NumericValueJSON<int64_t>>(value);
}
inline std::shared_ptr<MiniData_JSONImpl::BaseJSON> JSON::fromString(const std::string& value) {
  return std::make_shared<MiniData_JSONImpl::StringJSON>(value);
}
inline std::shared_ptr<MiniData_JSONImpl::BaseJSON> JSON::fromArray(const std::vector<JSON>& elems) {
  return std::make_shared<MiniData_JSONImpl::ArrayJSON>(elems);
}
inline std::shared_ptr<MiniData_JSONImpl::BaseJSON> JSON::fromMap(const std::map<std::string, JSON>& elems) {
  return std::make_shared<MiniData_JSONImpl::ObjectJSON>(elems);
}

inline JSON JSON::array(std::initializer_list<JSON> elems) {
  return JSON(std::vector<JSON>(elems));
}
inline JSON JSON::object(std::initializer_list<std::pair<const std::string, JSON>> elems) {
  return JSON(std::map<std::string, JSON>(elems));
}

/***************************************************************************/
/***********       Implementation of JSON::const_iterator        ***********/
/***************************************************************************/
inline JSON::const_iterator::const_iterator() {
    // Leave mImpl uninitialized
}
inline JSON::const_iterator::const_iterator(std::shared_ptr<MiniData_JSONImpl::JSONSource> source) : mImpl(source) {

}

/* We support a minimal equality comparison that makes any iterator compare equal to itself
 * and any two iterators at the end of the range compare equal.
 */
inline bool JSON::const_iterator::operator== (const_iterator rhs) const {
    /* Case 1: Both iterators are null. */
    if (!mImpl && !rhs.mImpl) return true;
    
    /* Case 2: We're null, they aren't. */
    else if (!mImpl) return rhs.mImpl->finished();
    
    /* Case 3: They're null, we aren't. */
    else if (!rhs.mImpl) return mImpl->finished();
    
    /* Case 4: Neither is null. */
    return mImpl == rhs.mImpl;
}

inline bool JSON::const_iterator::operator!= (const_iterator rhs) const {
    return !(*this == rhs);
}
    
inline JSON::const_iterator& JSON::const_iterator::operator++ () {
    mImpl->advance();
    return *this;
}

inline const JSON::const_iterator JSON::const_iterator::operator++ (int) {
    auto result = *this;
    ++*this;
    return result;
}

inline const JSON& JSON::const_iterator::operator* () const {
    return mImpl->current();
}
    
inline const JSON* JSON::const_iterator::operator-> () const {
    return &**this;
}

inline JSON::const_iterator JSON::begin() const {
    return MiniData_JSONImpl::JSONSource::make(MiniData_JSONImpl::as<MiniData_JSONImpl::IterableJSON>(mImpl)->source());
}
inline JSON::const_iterator JSON::end() const {
    return {};
}
inline JSON::const_iterator JSON::cbegin() const {
    return begin();
}
inline JSON::const_iterator JSON::cend() const {
    return end();
}

/***************************************************************************/
/***********         Implementation of parsing routines          ***********/
/***************************************************************************/

/* String parsing just wraps things as an istringstream and forwards the call. */
inline JSON JSON::parse(const std::string& input) {
    std::istringstream converter(input);
    return parse(converter);
}

namespace MiniData_JSONImpl {
    /* Utility function to report an parsing error. */
    [[ noreturn ]] inline void parseError(const std::string& reason) {
        jsonError("JSON Parse Error: " + reason);
    }

    /* Utility function to confirm the next character matches a specific value. */
    inline void expect(std::istream& input, char32_t ch) {
        char32_t found = readChar(input);
        if (found != ch) parseError("Expected " + toUTF8(ch) + ", got " + toUTF8(found));
    }
    
    /* This works one char at a time, so this will only work with strings of base ASCII characters. */
    inline void expect(std::istream& input, const std::string& str) {
        for (char ch: str) {
            expect(input, ch);
        }
    }

    /* All of these parsing routines use the grammar specified on the JSON website
     * (https://www.json.org/). This is a top-down, recursive-descent parser.
     */
    inline JSON readObject(std::istream& input);
    inline JSON readElement(std::istream& input);
    inline JSON readArray(std::istream& input);
    inline JSON readNumber(std::istream& input);
    inline std::string readString(std::istream& input);

    inline std::nullptr_t readNull(std::istream& input) {
        expect(input, "null");
        return nullptr;
    }

    inline bool readBoolean(std::istream& input) {
        if (peekChar(input) == 't') {
            expect(input, "true");
            return true;
        } else if (peekChar(input) == 'f') {
            expect(input, "false");
            return false;
        } else {
            parseError("Can't parse a boolean starting with " + toUTF8(peekChar(input)));
        }
    }
    
    inline bool isDigit(char32_t ch) {
        return ch >= '0' && ch <= '9';
    }

    inline std::string readDigits(std::istream& input) {
        std::ostringstream result;

        /* There must be at least one digit. */
        char32_t digit = readChar(input);
        if (!isDigit(digit)) {
            parseError("Expected a digit, got " + std::string(1, digit));
        }

        result << toUTF8(digit);

        /* Keep reading characters until we hit something that isn't a digit. */
        while (isDigit(peekChar(input))) {
            result << toUTF8(readChar(input));
        }

        return result.str();
    }

    inline std::string readInt(std::istream& input) {
        std::ostringstream result;

        /* There could potentially be a minus sign. */
        if (peekChar(input) == '-') {
            result << toUTF8(readChar(input));
        }

        /* There are two options here. First, we could be reading the number 0, in which
         * case we just read a 0. Nothing is permitted after this. Second, we could be
         * reading a multi-digit number, in which case that's what we'll go do.
         */
        if (peekChar(input) == '0') { // Just a zero
            result << toUTF8(readChar(input));
        } else {
            result << readDigits(input);
        }
        return result.str();
    }

    inline std::string readFrac(std::istream& input) {
        /* If the next character isn't a dot, there's nothing to read. */
        if (peekChar(input) != '.') return "";

        /* Otherwise, we should see a dot, then a series of digits. */
        std::ostringstream result;
        result << toUTF8(readChar(input));
        result << readDigits(input);
        return result.str();
    }

    inline std::string readExp(std::istream& input) {
        /* If the next character isn't e or E, there's nothing to read. */
        if (peekChar(input) != 'E' && peekChar(input) != 'e') return "";

        std::ostringstream result;
        result << toUTF8(readChar(input));

        /* There may optionally be a sign. */
        if (peekChar(input) == '+' || peekChar(input) == '-') {
            result << toUTF8(readChar(input));
        }

        /* Now, read some digits. */
        result << readDigits(input);

        return result.str();
    }
    
    /* Attempts to parse a string into a numeric format of some sort. On success, returns
     * true. On failure, returns false and does not modify the outparameter.
     */
    template <typename T> bool parseValue(const std::string& str, T& result) {
        /* Try reading a value, failing if we can't even do that. */
        std::istringstream extractor(str);
        T readValue;
        
        if (extractor >> readValue, !extractor) return false;
        
        /* See if there's anything left over. */
        char leftover;
        if (extractor >> leftover) return false;
        
        result = readValue;
        return true;
    }

    inline JSON readNumber(std::istream& input) {
        auto intPart  = readInt(input);
        auto fracPart = readFrac(input);
        auto expPart  = readExp(input);
        
        /* Assemble the string comprising the number. */
        std::string numericString = intPart + fracPart + expPart;
        
        /* Attempt to assemble this as an integer first. */
        std::int64_t integerValue;
        if (parseValue(numericString, integerValue)) {
            return BaseJSON::make<NumericValueJSON<std::int64_t>>(integerValue);
        }
        
        /* Otherwise, try doing it as a double. */
        double doubleValue;
        if (parseValue(numericString, doubleValue)) {
            return BaseJSON::make<NumericValueJSON<double>>(doubleValue);
        }
        
        /* Otherwise, give up! */
        parseError("Cannot parse number " + numericString + " as either an integer or a double.");
    }

    inline JSON readValue(std::istream& input) {
        /* Determine what to read based on the next character of input. */
        char32_t next = peekChar(input);

        if (next == '{') return readObject(input);
        if (next == '[') return readArray(input);
        if (next == '"') return JSON(readString(input));
        if (next == '-' || isdigit(next)) return readNumber(input); // TODO: isdigit assumes ASCII?
        if (next == 't' || next == 'f') return JSON(readBoolean(input));
        if (next == 'n') return JSON(readNull(input));

        parseError("Not sure how to handle value starting with character " + toUTF8(next));
    }

    inline std::string readString(std::istream& input) {
        std::string result;

        expect(input, '"');

        /* Keep reading characters as we find them. */
        while (true) {
            char32_t next = readChar(input);
            
            /* Only a certain character range is valid. */
            if (next < 0x20 || next > 0x10FFFF) parseError("Illegal character: " + toUTF8(next));

            /* We're done if this is a close quote. */
            if (next == '"') return result;

            /* If this isn't an escape sequence, just append it. */
            if (next != '\\') result += toUTF8(next);

            /* Otherwise, read it as an escape. */
            else {
                char32_t escaped = readChar(input);
                if      (escaped == '"')  result += '"';
                else if (escaped == '\\') result += '\\';
                else if (escaped == '/')  result += '/';
                else if (escaped == 'b')  result += '\b';
                else if (escaped == 'n')  result += '\n';
                else if (escaped == 'r')  result += '\r';
                else if (escaped == 't')  result += '\t';
                else if (escaped == 'u') {
                    input.unget();
                    input.unget();
                    result += toUTF8(readUTF16EscapedChar(input));
                } else parseError("Unknown escape sequence: \\" + toUTF8(escaped));
            }
        }
    }

    using Member = std::map<std::string, JSON>::value_type;
    inline Member readMember(std::istream& input) {
        input >> std::ws;
        auto key = readString(input);
        input >> std::ws;

        expect(input, ':');

        auto value = readElement(input);

        return { key, value };
    }

    inline JSON readArray(std::istream& input) {
        expect(input, '[');

        std::vector<JSON> elems;

        /* Edge case: This could be an empty array. */
        input >> std::ws;
        if (peekChar(input) == ']') {
            readChar(input); // Consume ']'
            return JSON(elems);
        }

        /* Otherwise, it's a nonempty list. */
        while (true) {
            elems.push_back(readElement(input));

            /* The next character should either be a comma or a close bracket. We stop
             * on a close bracket and continue on a comma.
             */
            char32_t next = readChar(input);
            if (next == ']') return JSON(elems);
            if (next != ',') parseError("Expected , or ], got " + toUTF8(next));
        }
    }

    inline JSON readObject(std::istream& input) {
        expect(input, '{');

        std::map<std::string, JSON> elems;

        /* Edge case: This could be an empty object. */
        input >> std::ws;
        if (peekChar(input) == '}') {
            readChar(input); // Consume '}'
            return JSON(elems);
        }

        /* Otherwise, it's a nonempty object. */
        while (true) {
            auto result = elems.insert(readMember(input));
            if (!result.second) parseError("Duplicate key: " + result.first->first);

            /* The next character should either be a comma or a close brace. We stop
             * on a close brace and continue on a comma.
             */
            char32_t next = readChar(input);
            if (next == '}') return JSON(elems);
            if (next != ',') parseError("Expected , or }, got " + toUTF8(next));
        }
    }

    inline JSON readElement(std::istream& input) {
        input >> std::ws;
        auto result = readValue(input);
        input >> std::ws;
        return result;
    }
}

/* Main parsing routine. */
inline JSON JSON::parse(std::istream& input) {
    auto result = MiniData_JSONImpl::readElement(input);

    /* Confirm there's nothing left in the stream. */
    char leftover;
    input >> leftover;
    if (input) MiniData_JSONImpl::parseError("Unexpected character found at end of stream: " + std::string(1, leftover));

    return result;
}

/* Stream extraction hooks into the main parsing routine. */
inline std::istream& operator>> (std::istream& in, JSON& j) {
    if (std::istream::sentry(in)) {
        try {
            auto result = MiniData_JSONImpl::readElement(in);
            j = std::move(result);
        } catch (const JSONException &) {
            in.setstate(std::ios::failbit);
        }
    }
    return in;
}

/***************************************************************************/
/***********         Implementation of exception types           ***********/
/***************************************************************************/
inline JSONException::JSONException(const std::string& reason): logic_error(reason) {
    // Handled in initializer list
}

#endif

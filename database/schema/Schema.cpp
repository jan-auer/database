#include <sstream>

#include "Schema.h"

using namespace std;

namespace lsql {

	static string type(const Attribute& attr) {
		Types::Tag type = attr.type;
		switch(type) {
      case Types::Tag::Integer:
				return "Integer";
				/*case Types::Tag::Numeric: {
         stringstream ss;
         ss << "Numeric(" << attr.len1 << ", " << attr.len2 << ")";
         return ss.str();
				 }*/
      case Types::Tag::Char: {
				stringstream ss;
				ss << "Char(" << attr.len << ")";
				return ss.str();
      }
		}
		throw;
	}

	string Schema::toString() const {
		stringstream out;
		for (const Relation& rel : relations) {
      out << rel.name << endl;
      out << "\tPrimary Key:";
      for (unsigned keyId : rel.primaryKey)
				out << ' ' << rel.attributes[keyId].name;
      out << endl;
      for (const auto& attr : rel.attributes)
				out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << endl;
		}
		return out.str();
	}

}

#include <visualization/server/escape.hpp>

#include <core/data/flexible_type/string_escape.hpp>

namespace turi {
namespace visualization {


std::string escape_float(flex_float value) {
  if (std::isnan(value)) {
    return "\"nan\"";
  }

  if (std::isinf(value)) {
    if (value > 0) {
      return "\"inf\"";
    } else {
      return "\"-inf\"";
    }
  }
  return std::to_string(value);
}


std::string escapeForTable( const flexible_type& value,
                            size_t row_index,
                            const std::string& columnName ){

    using namespace turi;
    using namespace turi::visualization;
    using namespace boost;
    using namespace local_time;
    using namespace gregorian;
    using posix_time::time_duration;

    static time_zone_names empty_timezone("", "", "", "");
    static time_duration empty_utc_offset(0,0,0);
    static dst_adjustment_offsets empty_adj_offsets(time_duration(0,0,0),
                                              time_duration(0,0,0),
                                              time_duration(0,0,0));
    static time_zone_ptr empty_tz(
        new custom_time_zone(empty_timezone, empty_utc_offset,
                              empty_adj_offsets,
                              boost::shared_ptr<dst_calc_rule>()));

    std::stringstream ss;

    switch (value.get_type()) {
      case flex_type_enum::UNDEFINED:
        ss << "null";
        break;
      case flex_type_enum::FLOAT:
        {
          flex_float f = value.get<flex_float>();
          if (std::isnan(f)) {
            ss << "\"nan\"";
            break;
          }
          if (std::isinf(f)) {
            if (f > 0) {
              ss << "\"inf\"";
            } else {
              ss << "\"-inf\"";
            }
            break;
          }
        }
      case flex_type_enum::INTEGER:
        ss << value;
        break;
      case flex_type_enum::DATETIME:
        {
          ss << "\"";
          const auto& dt = value.get<flex_date_time>();

          if (dt.time_zone_offset() != flex_date_time::EMPTY_TIMEZONE) {
            std::string prefix = "0.";
            int sign_adjuster = 1;
            if(dt.time_zone_offset() < 0) {
              sign_adjuster = -1;
              prefix = "-0.";
            }

            boost::local_time::time_zone_ptr zone(
                new boost::local_time::posix_time_zone(
                    "GMT" + prefix +
                    std::to_string(sign_adjuster *
                                   dt.time_zone_offset() *
                                   flex_date_time::TIMEZONE_RESOLUTION_IN_MINUTES)));
            boost::local_time::local_date_time az(
                flexible_type_impl::ptime_from_time_t(dt.posix_timestamp(),
                                                      dt.microsecond()), zone);
            ss << az;
          } else {
            boost::local_time::local_date_time az(
                flexible_type_impl::ptime_from_time_t(dt.posix_timestamp(),
                                                      dt.microsecond()),
                empty_tz);
            ss << az;
          }
          ss << "\"";
        }
        break;
      case flex_type_enum::VECTOR:
        {
          const flex_vec& vec = value.get<flex_vec>();

          ss << "[";
          for (size_t i = 0; i < vec.size(); ++i) {
            ss << vec[i];
            if (i + 1 < vec.size()) ss << ", ";
          }
          ss << "]";
        }
        break;
      case flex_type_enum::LIST:
        {
          const flex_list& vec = value.get<flex_list>();
          ss << "[";

          for (size_t i = 0; i < vec.size(); ++i) {
            const flexible_type val = vec[i];
            ss << turi::visualization::escapeForTable(val);
            if (i + 1 < vec.size()) ss << ", ";
          }

          ss << "]";
          break;
        }
      case flex_type_enum::DICT:
        {
          ss << "{";
          flex_dict dict = value.get<flex_dict>();
          size_t i = 0;
          for (const auto& pair : dict) {
            const auto& dict_key = pair.first;
            const auto& dict_value = pair.second;
            ss << turi::visualization::extra_label_escape(std::string(dict_key));
            ss << " : ";
            ss << turi::visualization::escapeForTable(dict_value);
            if (i + 1 < dict.size()) ss << ", ";
            i++;
          }
          ss << "}";
        }
        break;
      case flex_type_enum::ND_VECTOR:
        {
          flex_nd_vec val = value.get<flex_nd_vec>();
          std::vector<size_t> idx(val.shape().size(), 0);
          if (val.num_elem() == 0) ss << "[]";

          for (size_t i = 0;i < idx.size(); ++i) ss << "[";

          size_t next_bracket_depth;
          bool is_first_element = true;
          do {
            if (is_first_element == false) ss << ",";
            ss << escape_float(val[val.fast_index(idx)]);
            is_first_element = false;
            next_bracket_depth = val.increment_index(idx);
            if (next_bracket_depth == 0) break;
            for (size_t i = next_bracket_depth ;i < idx.size(); ++i) ss << "]";
            if (next_bracket_depth < idx.size()) ss << ",";
            for (size_t i = next_bracket_depth;i < idx.size(); ++i) ss << "[";
            if (next_bracket_depth < idx.size()) is_first_element = true;
          }while(1);
          for (size_t i = 0;i < idx.size(); ++i) ss << "]";
        }
        break;
      default:
        {
          ss << turi::visualization::extra_label_escape(value.to<std::string>());
        }
        break;
    }

    return ss.str();
}

std::string escape_string(const std::string& str, bool include_quotes) {
  std::string ret;
  size_t ret_len;
  ::turi::escape_string(str, '\\', true /* use_escape_char */, '\"', include_quotes /* use_quote_char */, false /* double_quote */, ret, ret_len);

  // ::turi::escape_string may yield an std::string padded with null terminators, and ret_len represents the true length.
  // truncate to the ret_len length.
  ret.resize(ret_len);
  DASSERT_EQ(ret.size(), strlen(ret.c_str()));

  return ret;
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string extra_label_escape(const std::string& str, bool include_quotes){
  std::string escaped_string = escape_string(str, include_quotes /* include_quotes */);
  escaped_string = replace_all(escaped_string, std::string("\\n"), std::string("\\\\n"));
  escaped_string = replace_all(escaped_string, std::string("\\t"), std::string("\\\\t"));
  escaped_string = replace_all(escaped_string, std::string("\\b"), std::string("\\\\b"));
  escaped_string = replace_all(escaped_string, std::string("\\r"), std::string("\\\\r"));
  escaped_string = replace_all(escaped_string, std::string("\\x"), std::string("\\\\x"));

  return escaped_string;
}

} // visualization
} // turi

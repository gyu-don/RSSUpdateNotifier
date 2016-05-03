struct DateTime {
  short year;
  signed char month;
  signed char day;
  signed char hour;
  signed char minute;

  static int compare(const DateTime &lhs, const DateTime &rhs);
  bool operator<(const DateTime &rhs) const {
    return compare(*this, rhs) < 0;
  }
  bool operator>(const DateTime &rhs) const {
    return compare(*this, rhs) > 0;
  }
  bool operator<=(const DateTime &rhs) const {
    return compare(*this, rhs) <= 0;
  }
  bool operator>=(const DateTime &rhs) const {
    return compare(*this, rhs) >= 0;
  }
  String toString();
};

// compare 2 DateTime and return integer as following
// dt1 < dt2: return negative value
// dt1 > dt2: return positive value
// dt1 == dt2: return zero.
int DateTime::compare(const DateTime &dt1, const DateTime &dt2)
{
  int t;
  (t = dt1.year - dt2.year) ||
  (t = dt1.month - dt2.month) ||
  (t = dt1.day - dt2.day) ||
  (t = dt1.hour - dt2.hour) ||
  (t = dt1.minute - dt2.minute);
  return t;
}

String DateTime::toString()
{
  String s(day);

  switch(month){
    case 1: s += " Jan "; break;
    case 2: s += " Feb "; break;
    case 3: s += " Mar "; break;
    case 4: s += " Apr "; break;
    case 5: s += " May "; break;
    case 6: s += " Jun "; break;
    case 7: s += " Jul "; break;
    case 8: s += " Aug "; break;
    case 9: s += " Sep "; break;
    case 10: s += " Oct "; break;
    case 11: s += " Nov "; break;
    case 12: s += " Dec "; break;
    default: s += " ??? ";
  }
  s += year;
  s += " ";
  s += hour;
  s += minute < 10 ? ":0" : ":";
  s += minute;
  return s;
}

// From Data and Time string to DateTime.
// Date and Time Specification is based on RFC822,
// with the exceptions that the year is expressed with 4 chars.
// Second and Time zone is ignored.
//
// [day "," ] 1*2DIGIT month 4DIGIT 2DIGIT ":" 2DIGIT [":" 2DIGIT]
// If succeed return true, otherwise return false
bool str_to_datetime(const String s, DateTime &dt){
  int i = 0;
  int len = s.length();
  int ibuf;

  //Serial.println(s); dt.year = 9999; dt.month = 99; dt.day = 99; dt.hour = 99; dt.minute = 99;
  while(i < len && isWhitespace(s[i])) i++;
  if(i < len && isAlpha(s[i])){
    // it may have "Mon,"  "Tue," ...; Ignore this.
    if(isAlpha(s[i+1]) && isAlpha(s[i+2]) && s[i+3] == ','){
      i += 4;
      while(i < len && isWhitespace(s[i])) i++;
    }
    else return false;
  }
  //for(int t=0;t<i;t++) Serial.print(" "); Serial.println(s.substring(i)); Serial.println(dt.toString());

  // day
  if(i >= len || !isDigit(s[i])) return false;
  ibuf = s[i++] - '0';
  if(i < len && isDigit(s[i])) ibuf = ibuf * 10 + (s[i++] - '0');
  dt.day = static_cast<signed char>(ibuf);
  while(i < len && isWhitespace(s[i])) i++;
  //for(int t=0;t<i;t++) Serial.print(" "); Serial.println(s.substring(i)); Serial.println(dt.toString());

  // month
  if(i+3 >= len) return false;
  String sbuf = s.substring(i, i+3);
  if(sbuf.equals("Jan")) dt.month = 1;
  else if(sbuf.equals("Feb")) dt.month = 2;
  else if(sbuf.equals("Mar")) dt.month = 3;
  else if(sbuf.equals("Apr")) dt.month = 4;
  else if(sbuf.equals("May")) dt.month = 5;
  else if(sbuf.equals("Jun")) dt.month = 6;
  else if(sbuf.equals("Jul")) dt.month = 7;
  else if(sbuf.equals("Aug")) dt.month = 8;
  else if(sbuf.equals("Sep")) dt.month = 9;
  else if(sbuf.equals("Oct")) dt.month = 10;
  else if(sbuf.equals("Nov")) dt.month = 11;
  else if(sbuf.equals("Dec")) dt.month = 12;
  else return false;
  i += 3;
  while(i < len && isWhitespace(s[i])) i++;
  //for(int t=0;t<i;t++) Serial.print(" "); Serial.println(s.substring(i)); Serial.println(dt.toString());

  // year
  if(i+4 >= len) return false;
  sbuf = s.substring(i, i+4);
  if(!isDigit(sbuf[0]) || !isDigit(sbuf[1]) || !isDigit(sbuf[2]) || !isDigit(sbuf[3])) return false;
  i += 4;
  dt.year = sbuf.toInt();
  while(i < len && isWhitespace(s[i])) i++;
  //for(int t=0;t<i;t++) Serial.print(" "); Serial.println(s.substring(i)); Serial.println(dt.toString());

  // hour
  if(i+3 >= len) return false;
  sbuf = s.substring(i, i+2);
  if(!isDigit(sbuf[0]) || !isDigit(sbuf[1])) return false;
  i += 2;
  dt.hour = sbuf.toInt();
  if(s[i] != ':') return false;
  i++;
  //for(int t=0;t<i;t++) Serial.print(" "); Serial.println(s.substring(i)); Serial.println(dt.toString());

  //minute
  if(i+3 >= len) return false;
  sbuf = s.substring(i, i+2);
  if(!isDigit(sbuf[0]) || !isDigit(sbuf[1])) return false;
  i += 2;
  dt.minute = sbuf.toInt();
  //for(int t=0;t<i;t++) Serial.print(" "); Serial.println(s.substring(i)); Serial.println(dt.toString());
  return true;
}


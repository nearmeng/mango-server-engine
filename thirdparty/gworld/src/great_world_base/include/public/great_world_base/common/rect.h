#pragma once
#include <algorithm>
#include <sstream>
#include "great_world_base/common/math_utility.h"
#include "great_world_base/common/shm/shm_stl.h"

namespace gw {

class AxisRange {
 public:
  void Update(float min_val, float max_val) {
    data_[0] = min_val;
    data_[1] = max_val;
  }

  float& operator[](int i) { return data_[i]; }
  float operator[](int i) const { return data_[i]; }

  float length() const { return data_[1] - data_[0]; }

  float center() const { return (max_ + min_) / 2.f; }

  void InflateBy(float value) {
    min_ -= value;
    max_ += value;
    min_ = std::min(min_, max_);
    max_ = std::max(min_, max_);
  }

  bool Contains(float pt) const { return gw::MathUtility::IsMoreEqu(pt, min_) && gw::MathUtility::IsMoreEqu(max_, pt); }
  bool Contains(const AxisRange& range) const {
    return gw::MathUtility::IsMoreEqu(range.min_, min_) && gw::MathUtility::IsMoreEqu(max_, range.max_);
  }

  float Distance(float pt) const { return std::max(pt - max_, min_ - pt); }

  void Scale(float scale) {
    min_ *= scale;
    max_ *= scale;
  }

 private:
  union {
    float data_[2];
    struct {
      float min_, max_;
    };
  };
};

class Rect {
 public:
  Rect(float minx, float miny, float maxx, float maxy) {
    x_.Update(minx, maxx);
    y_.Update(miny, maxy);
  }

  Rect(const Rect& proto) {
    minx_ = proto.minx();
    miny_ = proto.miny();
    maxx_ = proto.maxx();
    maxy_ = proto.maxy();
  }

  Rect() : minx_(0), maxx_(0), miny_(0), maxy_(0){};

  float Area() const { return x_.length() * y_.length(); }

  bool HasArea() const { return minx_ < maxx_ && miny_ < maxy_; }

  void InflateBy(float value) {
    x_.InflateBy(value);
    y_.InflateBy(value);
  }

  void Scale(float scale) {
    x_.Scale(scale);
    y_.Scale(scale);
  }

  bool Intersects(const Rect& rect) const {
    return gw::MathUtility::IsMoreEqu(maxx_, rect.minx_) &&
           gw::MathUtility::IsMoreEqu(rect.maxx_, minx_) &&
           gw::MathUtility::IsMoreEqu(maxy_, rect.miny_) &&
           gw::MathUtility::IsMoreEqu(rect.maxy_, miny_);
  }

  float IntersectsArea(const Rect& rect) const {
    if (!Intersects(rect)) {
      return 0;
    }
    float minMaxX = gw::MathUtility::IsMore(maxx_, rect.maxx_) ? rect.maxx_ : maxx_;
    float maxMinX = gw::MathUtility::IsMore(rect.minx_, minx_) ? rect.minx_ : minx_;
    float minMaxY = gw::MathUtility::IsMore(maxy_, rect.maxy_) ? rect.maxy_ : maxy_;
    float maxMinY = gw::MathUtility::IsMore(miny_, rect.miny_) ? miny_ : rect.miny_;
    return (minMaxX - maxMinX) * (minMaxY - maxMinY);
  }

  void IntersectsRect(const Rect& r) {
      minx_ = std::max(minx_, r.minx_);
      miny_ = std::max(miny_, r.miny_);
      maxx_ = std::min(maxx_, r.maxx_);
      maxy_ = std::min(maxy_, r.maxy_);
  }

  bool Contains(float x, float y) const {
    return x_.Contains(x) && y_.Contains(y);
  }

  bool Equal(const Rect& rect) const {
    return gw::MathUtility::IsEqual(maxx_, rect.maxx()) &&
           gw::MathUtility::IsEqual(maxy_, rect.maxy()) &&
           gw::MathUtility::IsEqual(rect.minx(), minx_) &&
           gw::MathUtility::IsEqual(rect.miny(), miny_);
  }

  float Distance(float x, float y) const {
    return std::max(x_.Distance(x), y_.Distance(y));
  }

  void Merge(const Rect& r1, const Rect& r2) {
    minx_ = std::min(r1.minx_, r2.minx_);
    miny_ = std::min(r1.miny_, r2.miny_);
    maxx_ = std::max(r1.maxx_, r2.maxx_);
    maxy_ = std::max(r1.maxy_, r2.maxy_);
  }

  void Merge(const Rect& r) {
    minx_ = std::min(minx_, r.minx_);
    miny_ = std::min(miny_, r.miny_);
    maxx_ = std::max(maxx_, r.maxx_);
    maxy_ = std::max(maxy_, r.maxy_);
  }

  AxisRange& GetRange(bool gety) { return gety ? y_ : x_; }
  const AxisRange& GetRange(bool gety) const { return gety ? y_ : x_; }

  float minx() const { return minx_; }
  float maxx() const { return maxx_; }
  float miny() const { return miny_; }
  float maxy() const { return maxy_; }

  float min(bool gety) const { return gety ? miny_ : minx_; }
  float max(bool gety) const { return gety ? maxy_ : maxx_; }
  void set_min(float value, bool sety) {
    if (sety)
      miny_ = value;
    else
      minx_ = value;
  }
  void set_max(float value, bool sety) {
    if (sety)
      maxy_ = value;
    else
      maxx_ = value;
  }

  void reset(Rect* other = nullptr) {
    if (other == nullptr) {
      minx_ = 0;
      maxx_ = 0;
      miny_ = 0;
      maxy_ = 0;
    } else {
      minx_ = other->minx_;
      maxx_ = other->maxx_;
      miny_ = other->miny_;
      maxy_ = other->maxy_;
    }
  }

  float width() const { return x_.length(); }
  float height() const { return y_.length(); }
  float length(bool gety) const { return gety ? y_.length() : x_.length(); }

  void set_minx(float value) { minx_ = value; }
  void set_maxx(float value) { maxx_ = value; }
  void set_miny(float value) { miny_ = value; }
  void set_maxy(float value) { maxy_ = value; }

  const AxisRange& rangex() const { return x_; }
  const AxisRange& rangey() const { return y_; }

  GWString DebugString() const {
    std::stringstream ss;
    ss << "minx:" << minx_ << ","
       << "maxx:" << maxx_ << ","
       << "miny:" << miny_ << ","
       << "maxy:" << maxy_;
    return GWString(ss.str().c_str(), ss.str().size());
  }

  union {
    struct {
      float minx_, maxx_;
    };
    AxisRange x_;
  };
  union {
    struct {
      float miny_, maxy_;
    };
    AxisRange y_;
  };
};

}  // namespace gw

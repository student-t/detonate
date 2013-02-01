#pragma once
#include <vector>
#include <string>
#include <boost/iterator.hpp>
#include "lazycsv.hh"
#include "line_stream.hh"
#include "alignment_segment.hh"

namespace detail
{
  class pslx_alignment_input_stream;
  class pslx_alignment_segments;
}

////////////////////////////////////////////////////////////////////////////
// pslx_alignment represents a single pslx alignment. It realizes the
// Alignment concept.
////////////////////////////////////////////////////////////////////////////

class pslx_alignment
{
public:
  // Realization of Alignment concept
  void parse_line(const std::string& line) { lazy_csv.parse_line(line); }
  std::string a_name()        const { return q_name(); }
  std::string b_name()        const { return t_name(); }
  double      frac_identity() const { return 1.0 * num_identity() / num_total(); }
  double      frac_indel()    const { return 1.0 * num_indel() / num_total(); }
  typedef detail::pslx_alignment_input_stream input_stream_type;
  typedef detail::pslx_alignment_segments     segments_type;
  segments_type segments() const; // defined below

  // For use by psl-specific algorithms:
  int                      matches      () const { return lazy_csv.at <int>        ( 0); } // Number of bases that match that aren't repeats
  int                      mis_matches  () const { return lazy_csv.at <int>        ( 1); } // Number of bases that don't match
  int                      rep_matches  () const { return lazy_csv.at <int>        ( 2); } // Number of bases that match but are part of repeats
  int                      n_count      () const { return lazy_csv.at <int>        ( 3); } // Number of 'N' bases
  int                      q_num_insert () const { return lazy_csv.at <int>        ( 4); } // Number of inserts in query
  int                      q_base_insert() const { return lazy_csv.at <int>        ( 5); } // Number of bases inserted in query
  int                      t_num_insert () const { return lazy_csv.at <int>        ( 6); } // Number of inserts in target
  int                      t_base_insert() const { return lazy_csv.at <int>        ( 7); } // Number of bases inserted in target
  std::string              strand       () const { return lazy_csv.at <std::string>( 8); } // '+' or '-' for query strand. For translated 
                                                                                           // alignments, second '+'or '-' is for genomic strand
  std::string              q_name       () const { return lazy_csv.at <std::string>( 9); } // Query sequence name
  int                      q_size       () const { return lazy_csv.at <int>        (10); } // Query sequence size
  int                      q_start      () const { return lazy_csv.at <int>        (11); } // Alignment start position in query
  int                      q_end        () const { return lazy_csv.at <int>        (12); } // Alignment end position in query
  std::string              t_name       () const { return lazy_csv.at <std::string>(13); } // Target sequence name
  int                      t_size       () const { return lazy_csv.at <int>        (14); } // Target sequence size
  int                      t_start      () const { return lazy_csv.at <int>        (15); } // Alignment start position in target
  int                      t_end        () const { return lazy_csv.at <int>        (16); } // Alignment end position in target
  int                      block_count  () const { return lazy_csv.at <int>        (17); } // Number of blocks in the alignment (a block contains no gaps)
  std::vector<int>         block_sizes  () const { return parse_vector<int>        (18); } // Comma-separated list of sizes of each block
  std::vector<int>         q_starts     () const { return parse_vector<int>        (19); } // Comma-separated list of starting positions of each block in query
  std::vector<int>         t_starts     () const { return parse_vector<int>        (20); } // Comma-separated list of starting positions of each block in target
  std::vector<std::string> q_segs       () const { return parse_vector<std::string>(21); } // Comma-separated list of segments in query
  std::vector<std::string> t_segs       () const { return parse_vector<std::string>(22); } // Comma-separated list of segments in query

  int num_total()    const { return q_size() - n_count(); } // subtract n_count b/c 'n' doesn't count as a match even if it is one
  int num_identity() const { return matches() + rep_matches(); }
  int num_indel()    const { return q_base_insert() + t_base_insert(); }

private:
  lazycsv<23, '\t'> lazy_csv;

  template<typename T>
  inline std::vector<T> parse_vector(size_t col) const
  {
    std::vector<T> out;
    std::string str = lazy_csv.at<std::string>(col);
    std::string::const_iterator i, j;
    std::string::const_iterator end = str.end();
    for (i = j = str.begin(); i != end; i = j) {
      for (; j != end && *j != ','; ++j) {}
      out.push_back(boost::lexical_cast<T>(std::string(i, j)));
      if (j != end)
        ++j; // skip past ',' (including trailing ',')
    }
    return out;
  }
};

namespace detail
{
  ////////////////////////////////////////////////////////////////////////////
  // pslx_alignment_input_stream is an input stream that produces
  // pslx_alignment objects, one per line of the input stream.
  ////////////////////////////////////////////////////////////////////////////

  class pslx_alignment_input_stream
  {
  public:
    pslx_alignment_input_stream(std::istream& is) : is(is), ls(is) { skip_header(); }

    pslx_alignment_input_stream& operator>>(pslx_alignment& al)
    {
      if (ls >> line)
        al.parse_line(line);
      return *this;
    }

    inline operator bool() { return ls; }
    inline bool operator!() { return !ls; }

  private:
    std::istream& is;
    line_stream ls;
    std::string line;

    void skip_header()
    {
      std::string line;
      getline(is, line);
      check(line, "psLayout version 3");
      getline(is, line);
      check(line, "");
      getline(is, line);
      check(line, "match\tmis- \trep. \tN's\tQ gap\tQ gap\tT gap\tT gap\tstrand\tQ        \tQ   \tQ    \tQ  \tT        \tT   \tT    \tT  \tblock\tblockSizes \tqStarts\t tStarts");
      getline(is, line);
      check(line, "     \tmatch\tmatch\t   \tcount\tbases\tcount\tbases\t      \tname     \tsize\tstart\tend\tname     \tsize\tstart\tend\tcount");
      getline(is, line);
      check(line, "---------------------------------------------------------------------------------------------------------------------------------------------------------------");
    }

    void check(const std::string& x, const std::string& y)
    {
      if (x != y)
        throw std::runtime_error("Bad PSL header: expected \n'" + y + "', got \n'" + x + "'.");
    }
  };

  ////////////////////////////////////////////////////////////////////////////
  // pslx_alignment_segment_iterator is a forward iterator over alignment
  // segments of a pslx alignment. This class should never be instantiated
  // directly by the end user. Instead, use the segments method of the
  // Alignment concept to get a pslx_alignment_segments proxy, and use that
  // proxy's begin and end methods to get an iterator pair.
  ////////////////////////////////////////////////////////////////////////////

  struct pslx_alignment_segment_iterator
    : public boost::iterator_facade<pslx_alignment_segment_iterator,
                                    const alignment_segment,
                                    std::forward_iterator_tag>
  {
    pslx_alignment_segment_iterator() : at_end(true) { /* std::cout << "in default constructor" << std::endl; */ }

    pslx_alignment_segment_iterator(const pslx_alignment& al_)
    : at_end(false),
      al(&al_),
      i(0),
      block_sizes(al->block_sizes()),
      a_starts(al->q_starts()), b_starts(al->t_starts()),
      a_segs  (al->q_segs  ()), b_segs  (al->t_segs  ()) ,
      a_length(al->q_size()),
      a_is_rc(al->strand() == "-")
    {
      assert(a_starts.size() == block_sizes.size());
      assert(b_starts.size() == block_sizes.size());
      assert(a_segs  .size() == block_sizes.size());
      assert(b_segs  .size() == block_sizes.size());
      assert(al->strand() == "-" || al->strand() == "+");
      increment();
    }

  private:
    friend class boost::iterator_core_access;

    // ITERATOR FACADE PRIVATE INTERFACE STUFF

    void increment()
    {
      // check for end
      if (i == block_sizes.size()) {
        at_end = true;
        return;
      }

      // segment start and end
      seg.a_start = a_is_rc ? (a_length - 1) - a_starts[i] : a_starts[i];
      seg.a_end   = a_is_rc ? seg.a_start - (block_sizes[i] - 1)
                            : seg.a_start + (block_sizes[i] - 1);
      seg.b_start = b_starts[i];
      seg.b_end   = b_starts[i] + (block_sizes[i] - 1);

      // look for mismatches
      seg.a_mismatches.clear();
      seg.b_mismatches.clear();
      int a_pos = seg.a_start;
      int b_pos = seg.b_start;
      for (size_t j = 0; j < a_segs[i].size(); ++j) {
        if (a_segs[i][j] != b_segs[i][j]) {
          seg.a_mismatches.push_back(a_pos);
          seg.b_mismatches.push_back(b_pos);
        }
        if (a_is_rc)
          --a_pos;
        else
          ++a_pos;
        ++b_pos;
      }

      if (a_is_rc)
        assert(a_pos == (int)(seg.a_end) - 1);
      else
        assert(a_pos == (int)(seg.a_end) + 1);
      assert(b_pos == (int)(seg.b_end) + 1);

      i += 1;
    }

    bool equal(const pslx_alignment_segment_iterator& other) const
    {
      if (!at_end && other.at_end)
        return false;
      else if (at_end && other.at_end)
        return true;
      else
        throw std::runtime_error("General equal operator is not implemented.");
    }

    const alignment_segment& dereference() const { return seg; }

    // BOOKKEEPING DATA

    bool at_end;
    const pslx_alignment *al;
    alignment_segment seg;

    size_t i;
    std::vector<int>         block_sizes;
    std::vector<int>         a_starts, b_starts;
    std::vector<std::string> a_segs,   b_segs;
    size_t                   a_length;
    bool                     a_is_rc;
  };

  ////////////////////////////////////////////////////////////////////////////
  // pslx_alignment_segments is a proxy class, which looks like a standard
  // container that can be iterated over.
  ////////////////////////////////////////////////////////////////////////////

  class pslx_alignment_segments
  {
  public:
    pslx_alignment_segments(const pslx_alignment& al) : al(al) {}
    typedef const alignment_segment&        const_reference;
    typedef pslx_alignment_segment_iterator const_iterator;
    const_iterator begin() const { return pslx_alignment_segment_iterator(al); }
    const_iterator end()   const { return pslx_alignment_segment_iterator(); }

  private:
    const pslx_alignment& al;
  };

} // namespace detail

pslx_alignment::segments_type pslx_alignment::segments() const
{
  return pslx_alignment::segments_type(*this);
}
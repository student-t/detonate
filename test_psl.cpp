#include <string>
#include <iostream>
#define BOOST_TEST_MODULE test_psl
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include "psl.hh"

BOOST_AUTO_TEST_CASE(alignment_concept)
{
  psl_alignment al;
  std::string line = "79	0	0	0	3	10	2	7	+	Locus_1_Transcript_1/48_Confidence_0.136_Length_1158	1158	268	357	Locus_1_Transcript_25/41_Confidence_0.222_Length_1952	1952	1866	1952	4	17,47,6,9,	268,286,335,348,	1866,1883,1931,1943,";
  al.parse_line(line);

  BOOST_CHECK_EQUAL(al.a_name(), "Locus_1_Transcript_1/48_Confidence_0.136_Length_1158");
  BOOST_CHECK_EQUAL(al.b_name(), "Locus_1_Transcript_25/41_Confidence_0.222_Length_1952");
  BOOST_CHECK_EQUAL(al.frac_identity(), 1.0*(79+0)/1158);
  BOOST_CHECK_EQUAL(al.frac_indel(), 1.0*(10+7)/1158);
}

BOOST_AUTO_TEST_CASE(single_interval_realistic_usage)
{
  psl_alignment al;
  std::string line = "37	2	0	0	0	0	0	0	+	comp11_c0_seq1	453	0	39	contig_166768	1464	707	746	1	39,	0,	707,";
  al.parse_line(line);
  std::string a = 
  // 012345678901234567890123456789012345678
    "TTTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTT" // a seg
    "TTTTTTTTGGTTTTGTCTTTTTTTAATAGTCATTCCAAGTATCCATGAAATAAGTGGTTACAGGAAGTCCCTCACCCTCCCAAAAGCCACCCCCACTCCTAAGAGGAGGATGGTCGCGTCCATGCCCTGAGTCCACCCCGGGGAAGGTGACAGCATTGCTTCTGTGTAAATTATGTACTGCAAAAATTTTTTTAAATCTTCCGCCTTAATACTTCATTTTTGTTTTTAATTTCTGAATGGCCCAGGTCTGAGGCCTCCCTTTTTTTTGTCCCCCCAACTTGATGTATGAAGGCTTTGGTCTCCCTGGGAGGGGGTTGAGGTGTTGAGGCAGCCAGGGCTGGCCTGTACACTGACTTGAGACCAATAAAAGTGCACACCTTACCTTACACAAACAAAAAAAAAAAAAAAAAAAAA";
  std::string b = "CTCCGCGCGGTGCATTCTGGGCATCGAGGCCGAGCCCGCCGCCGCCGTCGCCTAAGAGGAGCGAGGAGAGGCCGCGACCGGGCAGAGACCGCCGGAGTCGCCGCCGGAGAGGAGTCGACTCGCCAGCCGCCGCCAGCGAGGCCCGCCCACCCGTTTGTCTGGCCCTCTGCCCCGTTCACCATGCAGCCTGCTTCTGCAAAGTGGTACGATCGAAGGGACTATGTATTCATTGAATTTTGTGTTGAAGACAGTAAAGATGTTAATGTAAACTTTGAAAAATCCAAACTTACTTTCAGTTGTCTTGGAGGAAGCGATAATTTTAAGCATTTAAATGAAATTGATCTTTTTCATTGTATCGATCCAAATGATTCCAAGCATAAAAGAACAGACAGATCGATTTTATGTTGTTTGCGAAAAGGAGAATCCGGCCAGTCATGGCCTAGGTTAACAAAGGAAAGGGCAAAGATGATGGATCACATGGGTGGTGATGAGGATGTAGATTTACCAGAAGTAGATGGAGCAGATGATGATTCACAAGACAGTGATGATGAAAAGATGCCAGATCTGGAGTAAGGGCTGTTGTCATCGCCTGGATTTTGAGAAAGGAAAATAACTTCTGCAAGATTTCACAATTGAGAAAACCCCTGAGTCGATAGCTCTAAAGGCAGATATGCTGTACTTGCCTCCTCTAACCCAGTTGTCAACCT"
  // 789012345678901234567890123456789012345
    "TTTTTTTTCTTTTTCTTTTTTTTCTTTTTTTTTTTTTTT" // b seg
    "GAAAGGCTTCACTAAGGGTTGATTATGTACCATTGTGTGGGGCGATTTAAGTCAGCTAAGGCAGTATCCTTATGCATGAACATTTCCCATGATTCCATGAGGCAGTTGATCCTAGGCAACTGCTCCAGCAGCTCGGACAAATAATTGCATTTCATCTAAGTCTTTCATCATAACACAATTACTTACACATTAGGAAAGCTCTCAGCTTAGGGAAAGATGAAAGTTTTAGATCCTAGACCATGGATTTAAAGGAGGCGGAAACAAATTGGCTAATACCTTAAACTGATAACCTGCCATTCTTTCAGGATACCTTTCAAAATTATTCCACTAAAGTTTATTTTTCAAAAAAAAAAAATGTACTTTACATCATTGTATGTGATCACTTGTCAGTGTTCAGGTGTATTTTAGCAAAAACTAGTGGATGCCCTAACTTAGATGGTTTTTGAAGCCTATACACTTGGTATTGTTTGGCCCTTAAGCTTTTACATCTCTTAGCATGGAGGACAAAGAAAGCTGTACATTGTTGCTTGAGAGTATGTACATTTTAGACCAGATTTGTATTTGCACAGTCAGTATGGCAAATGAGTGGACAATGTTAAATACACTATTGGATTTTTTTTATTTTCTGTTTTGATTTCAGCTTATGCCCTGGCTGGAAAACCTCAATTTATGTTCATGACAGTGGGGATTTTTTTAAATGTCTACATTCTTTCTAATA";
  size_t i = 0;
  BOOST_FOREACH(const alignment_segment& seg, al.segments(a, b)) {
    BOOST_CHECK_EQUAL(seg.a_start, 0ul);
    BOOST_CHECK_EQUAL(seg.a_end,   0ul + 39 - 1);
    BOOST_CHECK_EQUAL(seg.b_start, 707ul);
    BOOST_CHECK_EQUAL(seg.b_end,   707ul + 39 - 1);
    BOOST_CHECK_EQUAL(seg.a_mismatches.size(), 2ul);
    BOOST_CHECK_EQUAL(seg.b_mismatches.size(), 2ul);
    BOOST_CHECK_EQUAL(seg.a_mismatches[0], 8ul);
    BOOST_CHECK_EQUAL(seg.b_mismatches[0], 715ul);
    BOOST_CHECK_EQUAL(seg.a_mismatches[1], 14ul);
    BOOST_CHECK_EQUAL(seg.b_mismatches[1], 721ul);
    ++i;
  }
  BOOST_CHECK_EQUAL(i, 1ul);
}

BOOST_AUTO_TEST_CASE(several_intervals_forward_strand)
{
  psl_alignment al;
  std::string line = "348	6	0	0	1	25	2	314	+	comp14_c0_seq2	1343	121	533	contig_160743	718	13	714	3	5,305,77,	121,126,456,	13,21,637,";
  std::string a = "CTCTTCCGATTTGCGGCGGGGCGAGTCGCGTCCACCCGCGAGCACAGCTTCTTTGCAGCTCCTTCGTTGCCGGTCCACACCCGCCACCAGTTCGCCATGGATGACGATATCGCTGCGCTGG"
    "TCGTC" // seg 0
    "TGTGGTTTTTGTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" // seg 1
    // 126 + 305 = 431, 456 - 431 = 25
    "TAAGGCCAACCGTGAAAAGATGACC"
    "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" // seg 2
    "TCGTACCACAGGCATTGTGATGGACTCCGGAGACGGGGTCACCCACACTGTGCCCATCTACGAGGGCTATGCTCTCCCTCACGCCATCCTGCGTCTGGACCTGGCTGGCCGGGACCTGACAGACTACCTCATGAAGATCCTGACCGAGCGTGGCTACAGCTTCACCACCACAGCTGAGAGGGAAATCGTGCGTGACATCAAAGAGAAGCTGTGCTATGTTGCTCTAGACTTCGAGCAGGAGATGGCCACTGCCGCATCCTCTTCCTCCCTGGAGAAGAGCTATGAGCTGCCTGACGGCCAGGTCATCACTATTGGCAACGAGCGGTTCCGATGCCCTGAGGCTCTTTTCCAGCCTTCCTTCTTGGGTATGGAATCCTGTGGCATCCATGAAACTACATTCAATTCCATCATGAAGTGTGACGTTGACATCCGTAAAGACCTCTATGCCAACACAGTGCTGTCTGGTGGTACCACCATGTACCCAGGCATTGCTGACAGGATGCAGAAGGAGATTACTGCTCTGGCTCCTAGCACCATGAAGATCAAGATCATTGCTCCTCCTGAGCGCAAGTACTCTGTGTGGATCGGTGGCTCCATCCTGGCCTCACTGTCCACCTTCCAGCAGATGTGGATCAGCAAGCAGGAGTACGATGAGTCCGGCCCCTCCATCGTGCACCGCAAGTGCTTCTAGGCGGACTGTTACTGAGCTGCGTTTTACACCCTTTCTTTGACAAAACCTAACTTGCGCAGAAAAAAAAAAAATAAGAGACAACATTGGCATGGCTTTGTTTTTTTAAATTTTTTTTAAAG";
  std::string b = "GAAATCGCCGCAC"
    "TCGTC" // seg 0
    "ATT"
    "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" // seg 1
    // 21 + 305 = 326, 637 - 326 = 311
    "CAAAGCTAACAGAGAGAAGATGACGCAGGTATGCTTGAGCCGGGACCTTGCCTCCCCTTGGGCGTGACGAGTCATTTTGTGTTGTGCCTCATTTCCTGACATTCAAGCGTTTCTTTGGTGTTTCTAGGGTTCTGTTCCTCTTCTCCTGGCATTTCCTCCCTGAAGCCTCCAGGTTTCTGTTTGCGTTTCTGCTTGCGTTCTGTTCTTTTTTTCTTCACACATCACATTTGTGGCATGCAGCATGCAGAGTGTGGGTGTGGGAGGCTCAGGGCCCCTGCTACTCACTCACTCAGGTGTTCTTTGTCTTTCCA"
    "ATTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTA" // seg 2
    "GCGC";
  al.parse_line(line);
  psl_alignment::segments_type segs = al.segments(a, b);
  psl_alignment::segments_type::const_iterator it = segs.begin(), end = segs.end();
  size_t block_sizes[] = {5,305,77};
  size_t a_starts[] = {121,126,456};
  size_t b_starts[] = {13,21,637};
  for (size_t i = 0; i < 3; ++it, ++i) {
    BOOST_CHECK(it != end);
    BOOST_CHECK_EQUAL(it->a_start, a_starts[i]);
    BOOST_CHECK_EQUAL(it->a_end,   a_starts[i] + block_sizes[i] - 1);
    BOOST_CHECK_EQUAL(it->b_start, b_starts[i]);
    BOOST_CHECK_EQUAL(it->b_end,   b_starts[i] + block_sizes[i] - 1);
    if (i == 0) {
      BOOST_CHECK_EQUAL(it->a_mismatches.size(), 0ul);
      BOOST_CHECK_EQUAL(it->b_mismatches.size(), 0ul);
    } else if (i == 1) {
      BOOST_CHECK_EQUAL(it->a_mismatches.size(), 4ul);
      BOOST_CHECK_EQUAL(it->b_mismatches.size(), 4ul);
      BOOST_CHECK_EQUAL(it->a_mismatches[0], a_starts[i]+1);
      BOOST_CHECK_EQUAL(it->b_mismatches[0], b_starts[i]+1);
      BOOST_CHECK_EQUAL(it->a_mismatches[1], a_starts[i]+3);
      BOOST_CHECK_EQUAL(it->b_mismatches[1], b_starts[i]+3);
      BOOST_CHECK_EQUAL(it->a_mismatches[2], a_starts[i]+4);
      BOOST_CHECK_EQUAL(it->b_mismatches[2], b_starts[i]+4);
      BOOST_CHECK_EQUAL(it->a_mismatches[3], a_starts[i]+10);
      BOOST_CHECK_EQUAL(it->b_mismatches[3], b_starts[i]+10);
    } else if (i == 2) {
      BOOST_CHECK_EQUAL(it->a_mismatches.size(), 2ul);
      BOOST_CHECK_EQUAL(it->b_mismatches.size(), 2ul);
      BOOST_CHECK_EQUAL(it->a_mismatches[0], a_starts[i]);
      BOOST_CHECK_EQUAL(it->b_mismatches[0], b_starts[i]);
      BOOST_CHECK_EQUAL(it->a_mismatches[1], a_starts[i] + block_sizes[i] - 1);
      BOOST_CHECK_EQUAL(it->b_mismatches[1], b_starts[i] + block_sizes[i] - 1);
    }
  }
  BOOST_CHECK(it == end);
}

std::string reverse_complement(const std::string& a_rc)
{
  std::string a(a_rc);
  reverse(a.begin(), a.end());
  for (std::string::iterator i = a.begin(); i != a.end(); ++i) {
    if (*i == 'T') *i = 'A';
    else if (*i == 'A') *i = 'T';
    else if (*i == 'C') *i = 'G';
    else if (*i == 'G') *i = 'C';
    else
      throw std::runtime_error("invalid complement");
  }
  return a;
}

BOOST_AUTO_TEST_CASE(several_intervals_reverse_strand)
{
  psl_alignment al;
  std::string line = "115	5	0	0	2	4	2	6	-	comp27_c0_seq1	1918	339	463	contig_85997	226	0	126	4	98,6,7,9,	1455,1555,1561,1570,	0,98,105,117,";
  std::string a_rc = "TTTTCTGACCAAGGTTCAATTTTACTATTCCAGACACTGGGTTATAAAAAGGGGGAGGGGCCCATTCCCGCCAAATCATCCTAGGAGTCCAGTTGCAGGTGACCACGTGTTGGCTCAGGAACAGCTAGGTGGCAGGCAGCCGCAGTGGGAGTGGCAGAACGATAGGGCGGTAGGTCCACCCCACGCTCTCTTCCTGCTAACAGTGAAACCTGAGCAGCCAGGTTTCAGGCTAGGGGGAGAGGTTACAAATGAGGACTAAGAAAAATCTGTTTTCTGAAATCATAAAGAGCTTCATTCCAGACTTGCAGGGATGTTCAACATACATAAATCAGTAAATGAGATCCACTGTATAAATGGACTCAATTCAAGAAAGCAGAAAGTAGATTCAGAAAATGCCTGTGGTGAAAACCAACAGCTCTTCATTATTAAAGCCCTGAAGACAACAGAAATACAAGGAACGTATTTCAATACAATAAAGGAAATATACAACAAGCCTGTAGCCAACGGAATGCTAACCGGAGAAAAACTGGAAGCATTTCTGCTAAAAATCAGGAAAGGGACATGGATACTTTCTCTCAGTACTTCTTTTCAGCTGAGTACTTGAAGTTTTTGCTACAGCAGTAAGGCAAGAGAAGGAGACAAGATGGATTCAAACAGGAAAGGAAGAAGTCAAGCAATATTCATTGGCAGATGATATCATTCTTTCTCTCTCTATAAAAGGCCCTAAATGCTACACTAGAAAACTCGTAGAGCTGATAAACACTTTCAGCCAAGTGTCAGGACACGATCTGCACATACAAAAAAACCAATAGTCTTCCTATAAGCAAATGACAAACATACTGAGGAAGAAATTGGGAGGAAAATCCCACTCTCAATACCTGCAAAATAATATCTTGGAATAAACCTAATGAATAAAGCAAAAGGCCTATACAATGAAAACTCTAAAACAATCAAAGAAGACCCAGAAGATGGAAAGACCACCTATGCTCATGGTCTAGTTTAATATTTTGACAATGGTCAGCTTATCAAAAAAAATTTATAGATTCATTGCAATTCCCATCAAAATTCCAATGCAATTCTTCACAGATTTTGAAAATAACAATCTTAAAATTCATATGGAAACACAAAAGATCTTAGGTAACCCAAACACCCCTGAACAATAGAGACACTGCTGAAGGGATTATCATACAGAGCCACAGTAAAAAAACAGCATGAGACTGGTATAAAAACAAGCTGACCAAGTTGAGATCCTCGAATAGAATTGAGGATCTTATCGAACAGGGAGAACTTCCAAAGATTTGTAGATCACTTTAATGATTCTCTCTATAGTGGGTAAAATACATCCTTGATATGAAGTCTACATTGGAATAAACTATATTTCTGGAAATTTAAAAATTGTACCATGGGCCGGGTGGTGGTGGCGCACGCCTTTAATCCCAGCACTTGGGAGGCAGA"
    // 1455:
    "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" // seg 0
    // 1553:
    "CA"
    "AAAGGG" // seg 1
    // 1561:
    "CCTCCCT" // seg 2
    // 1568:
    "TT"
    "TTTTTTTTT" // seg 3
    "TTGTACCATGATATCTTTTGTTTTGAAGAAAATATTCAAAGTTACCTGACTCCCTCTGTTTCTTTTCCACTGCAGATTCAATAAAAGCGAGTGAGATGCTTTCTCCCTAACAGCCCAATTTCAGCCCATAGTCCTAGGAAACTGAAGCAAAAGAGGCAGACAGACCTGTGGCTGCATAGATGCACTTTATTAGTGGCTTACTGATGGAGGGGTGGTGGGAAAATCAGCTGGGTCCTTACAAACACAGACATACTACTGGGCTAAGGCAGAATTTATTTCATCCTAGTTGAATGTACCTGGGTTGCCATAAGTGACTATCAGAACCAGACACATTCCAAA";
  std::string a = reverse_complement(a_rc);
  std::string b = 
    "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" // seg 0
    "GGGGGG" // seg 1
    // 104
    "A"
    "CCCCCCC" // seg 2
    // 112
    "AAAAC"
    "TTTTTTTTT" // seg 3
    "AAAACAAAGAAACAGAGATTCTCCCTCCCATCCGTACCTCCCAAGTCCACATTAAAGGCATGTTTCGCGATGCCTGATTTTACCAGACCAAAGTTTTTTA";
  al.parse_line(line);
  psl_alignment::segments_type segs = al.segments(a, b);
  psl_alignment::segments_type::const_iterator it = segs.begin(), end = segs.end();
  size_t block_sizes[] = {98,6,7,9};
  size_t a_starts[] = {1918-(1455+1),1918-(1555+1),1918-(1561+1),1918-(1570+1)};
  size_t b_starts[] = {0,98,105,117};
  for (size_t i = 0; i < 4; ++it, ++i) {
    BOOST_CHECK(it != end);
    BOOST_CHECK_EQUAL(it->a_start, a_starts[i]);
    BOOST_CHECK_EQUAL(it->a_end,   a_starts[i] - (block_sizes[i] - 1));
    BOOST_CHECK_EQUAL(it->b_start, b_starts[i]);
    BOOST_CHECK_EQUAL(it->b_end,   b_starts[i] + (block_sizes[i] - 1));
    if (i == 0 || i == 3) {
      BOOST_CHECK_EQUAL(it->a_mismatches.size(), 0ul);
      BOOST_CHECK_EQUAL(it->b_mismatches.size(), 0ul);
    } else if (i == 1) {
      BOOST_CHECK_EQUAL(it->a_mismatches.size(), 3ul);
      BOOST_CHECK_EQUAL(it->b_mismatches.size(), 3ul);
      BOOST_CHECK_EQUAL(it->a_mismatches[0], a_starts[i]);
      BOOST_CHECK_EQUAL(it->b_mismatches[0], b_starts[i]);
      BOOST_CHECK_EQUAL(it->a_mismatches[1], a_starts[i]-1);
      BOOST_CHECK_EQUAL(it->b_mismatches[1], b_starts[i]+1);
      BOOST_CHECK_EQUAL(it->a_mismatches[2], a_starts[i]-2);
      BOOST_CHECK_EQUAL(it->b_mismatches[2], b_starts[i]+2);
    } else if (i == 2) {
      BOOST_CHECK_EQUAL(it->a_mismatches.size(), 2ul);
      BOOST_CHECK_EQUAL(it->b_mismatches.size(), 2ul);
      BOOST_CHECK_EQUAL(it->a_mismatches[0], a_starts[i] - 2);
      BOOST_CHECK_EQUAL(it->b_mismatches[0], b_starts[i] + 2);
      BOOST_CHECK_EQUAL(it->a_mismatches[1], a_starts[i] - (block_sizes[i] - 1));
      BOOST_CHECK_EQUAL(it->b_mismatches[1], b_starts[i] + (block_sizes[i] - 1));
    }
  }
  BOOST_CHECK(it == end);
}
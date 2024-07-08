---
v: 3

title: On Numbers in CBOR
abbrev: CBOR Numbers
docname: draft-bormann-cbor-numbers-latest
category: info
stream: IETF

date:
consensus: true
area: "Applications and Real-Time"
workgroup: "Concise Binary Object Representation Maintenance and Extensions"
keyword:

venue:
  group: "Concise Binary Object Representation Maintenance and Extensions"
  mail: "cbor@ietf.org"
  github: cabo/cbor-numbers

author:
-   name: Carsten Bormann
    org: Universität Bremen TZI
    street: Postfach 330440
    city: Bremen
    code: D-28359
    country: Germany
    phone: +49-421-218-63921
    email: cabo@tzi.org
- name: Laurence Lundblade
  org: Security Theory LLC
  email: lgl@securitytheory.com

normative:
  STD94: cbor
#    =: RFC8949
  RFC8746: arrays
  IEEE754:
    target: https://ieeexplore.ieee.org/document/8766229
    title: IEEE Standard for Floating-Point Arithmetic
    author:
    - org: IEEE
    date: false
    seriesinfo:
      IEEE Std: 754-2019
      DOI: 10.1109/IEEESTD.2019.8766229
  RFC8610: cddl

informative:
  I-D.ietf-cbor-cde: cde
  I-D.bormann-cbor-det: det
  I-D.mcnally-deterministic-cbor: dcbor-orig

--- abstract

[^abs1-] STD 94 (RFC 8949), [^abs2-]

[^abs1-]:
    The Concise Binary Object Representation (CBOR), as defined in

[^abs2-]:
    is a data representation format whose design goals include the
    possibility of extremely small code size, fairly small message
    size, and extensibility without the need for version negotiation.

    Among the kinds of data that a data representation format needs to be
    able to carry, numbers have a prominent role, but also have
    inherent complexity that needs attention from implementers of CBOR
    libraries and the applications that use them.

    This document gives an overview over number formats available in
    CBOR and some notable CBOR tags registered, and it attempts to
    provide information about opportunities and potential pitfalls of these
    number formats.

[^disclaimer]

[^disclaimer]:  This is an initial revision, pieced together from
    various contributions, so it has a higher level of redundancy than
    ultimately desired.

--- middle

# Introduction


[^abs1-] {{RFC8949@-cbor}}, [^abs2-]

It discusses CBOR representation of numbers in four main Sections:

* {{<<sec-int}} ({{sec-int}}),
* {{<<sec-ieee}} ({{sec-ieee}}),
* {{<<sec-float}} ({{sec-float}}),
* {{<<sec-array}} ({{sec-array}}).

These sections will generally address considerations such as:

* Encoding efficiency (number of bytes needed)
* Preferred Serialization, Common Deterministic Encoding Profile (CDE,
  {{-cde}}, with more background discussion in {{-det}})
* Use by applications
* Interoperability considerations, potential "dark corners"

## Conventions and Definitions

{::boilerplate bcp14-tagged-bcp}

Terms and definitions from {{-cbor}} apply.

# Integer Numbers {#sec-int}

CBOR provides representations of integer numbers in unsigned and negative forms:

* Unsigned integers up to 2<sup>64</sup>−1, major type 0
* Negative integers down to −2<sup>64</sup>, major type 1
* Unsigned integers with no size limitations, tag 2 on a byte string
* Negative integers with no size limitations, tag 3 on a byte string

The latter two forms are often called "bignums" for historical
reasons, the former "basic" integers.  The Concise Data Definition
Language (CDDL) {{-cddl}} has the types `uint`,
`nint`, and `int`, for the ranges of values covered by major type 0,
major type 1, and either of them, respectively; `biguint`, `bignint`,
and `bigint` for the range of value covered by tag 2, tag3, and either;
and `unsigned` and `integer` for a choice of either form (but
interestingly no `negative`).
As the preferred encoding for an integer chooses between major type
0/1 and tag 2/3 automatically, in practice `biguint` and `unsigned`
are the same type, as are `bigint` and `integer`.

The Major type 0 numbers come in five different encoding sizes, as
indicated by their initial byte:
immediate ("1+0") encoding (0..23), one-byte ("1+1") (0..255),
two-byte ("1+2", 0..65535), four-byte, and eight-byte.
The Preferred Serialization uses the shortest of these encodings.
The intention is that there is no semantic difference between the
major type 0 encodings, and there also is no semantic difference between major type 0 and
tag 2.
This means that Preferred Serialization can always use major type 0
over tag 2 when possible, and the shortest encoding (and no initial
zero bytes for the tagged encodings)

Note that there is no "signed type" in CBOR: as any specific number to
be represented is either negative or not, it is represented as an
unsigned integer or as a negative integer.
Major type 0 unsigned integers cover exactly the range of platform
types such as `uint64_t` or `u64`.
Signed platform types such as `int64_t` or `i64` can be represented in
the lower half of the unsigned space and the upper half of the
negative space.
Platforms typically have no `nint64_t` type that could take all
negative numbers representable in major type 1; generic decoders will
therefore treat the lower half of the negative space in the same way
they will treat bignums that do not fit the signed platform type.
Similarly, generic encoders for a platform with `u128`/`i128` types
will choose between major type 0/1 and tag 2/3 just like it would
choose between the encoding sizes inside major type 0/1.

While additional representation of integers could be developed, the
options already provided by {{-cbor}} should be able to satisfy most
applications.

# IEEE 754 Floating Point Numbers {#sec-ieee}

While integer numbers are relatively easy to represent, floating point
numbers as a realization of rational or real numbers are a much more
varied subject.  Many rational or real numbers require rounding until
they can be encoded as a floating point number.

There are many choices that can be made when designing a machine
representation for floating point numbers.
After decades of vendor-specific formats, IEEE standardized the initial
version of {{IEEE754}} in 1985, updated in 2008 and 2019.
This standard is widely adopted, offering choices such as binary
vs. decimal floating point numbers, and different representation
sizes.
Out of the large choice available, CBOR directly supports binary16,
binary32, and binary64, signed binary floating point formats in 16,
32, and 64 bits, colloquially known as half (16 bits), single (32
bits), and double (64 bits) precision.
Most platforms that support floating point computation support at
least single precision, usually also double precision, while half
precision is mostly used for storage and interchange only.

## Integer vs. Floating Point

Mathematically speaking, integer numbers are a subset of the rational
or real numbers from which floating point numbers are drawn.
In many programming environments, however, integer numbers are clearly
separated from floating point numbers (the most notable exception
being the original JavaScript language).

For specific applications, it may be desirable to represent all
numbers that can be represented as integers as such, even if they are
in a position that also could be taken by a non-integer floating
point number.  {{-dcbor-orig}} defines a CDE application profile that
enforces this for a certain subset of the integers.

Most CBOR applications so far have tended to get by with the kind of
strong separation between the integer and floating point worlds that
programming environments usually favor, so we will not further pursue
approaches for intermingling them in this document.


## Considerations for non-finite numbers and non-numbers

IEEE754 distinguishes three kinds of floating point data item:

* finite floating-point number: A finite number that is representable
  in a floating-point format.  Note that these further divide into
  zero, subnormal, and normal; this distinction is usually not of
  interest in interchange, except that there are a few platforms with
  limited floating point support that may not support subnormal
  numbers.
* infinite floating-point number: -Infinite or +Infinite.
  On many platforms, infinite numbers can be accessed via a floating
  point operation such as 1.0/0.0 (positive infinity) or −1.0/0.0
  (negative infinity); they react to comparisons as one would expect.
* NaN: a _floating point datum_ that is not a number (NaN), represent
  computations that didn't lead to a numeric result, not even an
  infinity.
  A commonly implemented example for such a computation is 0.0/0.0.
  The formats provide a way to include additional information with a
  NaN, such as its sign bit, whether operations on the NaN are
  intended to fail immediately (signalling) or just return another NaN
  (quiet), and some remaining bits that may carry additional information.

  It can be surprising that, according to {{IEEE754}} NaN values always
  compare as different even if they have the same NaN information
  (i.e., are identical).

Not all platforms that use IEEE 754 do provide all these kinds, e.g.,
Erlang only provides finite floating-point numbers.
Platforms that do provide them widely vary in the way they provide
access to non-finite numbers and NaNs beyond the floating point
operations given above.
Usually there is an operation such as `isnan()` in C, which is needed
as comparison to a NaN always yields unequal.

### Protocol Design Considerations

CBOR supports the interchange of all kinds of IEEE 754 data items,
including non-finite numbers and non-numbers (NaNs).
For an application developer that is already using IEEE 754 floating
point, there is little additional consideration required:
Both infinities and NaN are widely supported in IEEE-754 hardware and
software by CPUs, OS’s and programming environments.
CBOR protocol designs can generally rely on infinities and NaN as a
concept being supported, but implementations may run into dark corners
of their platforms when it comes to distinguishing and preserving NaN
information in NaN values.

However, for a protocol that wants to achieve good interoperability
over a wide variety of platforms, the fact that platforms differ in
their support of non-finite numbers and NaNs becomes relevant.
(See {{implcons}} below for reasons for such differences.)
Protocol designs aiming for the widest possible platform support may
want to implement replacements for infinite numbers and NaNs, or at
least not rely on NaN information being successfully preserved during
interchange.

#### JSON Compatibility
{:unnumbered}

Note that JSON supports neither infinite numbers nor NaN.
For protocols that are intended to work in both CBOR and JSON
representations and need an out-of-band indicator comparable to NaN, a
protocol developer might consider this (in CDDL, where `float` is not
intended to be a NaN value):

~~~ cddl
float-with-null = float / null
~~~

Additional choices can be added for the infinities (e.g., `false` and
`true`, to stay within the CBOR simple values), if required.

Since `null`, `false` and `true` have single-byte representations, the
replacement of NaN, -Infinity, and +Infinity by these values can save
bytes even if JSON compatibility is not a consideration.

Applications that need to preserve the information in a NaN (sign bit,
quiet bit, payload) may want to replace `null` with an
application-oriented representation of that information, or simply
with a (left-aligned, truncating trailing zero bytes) byte string
representing those bits:

float-with-nan-replacement = float / bytes

For JSON, the byte string can be base16- or base64-encoded, or it can
be represented by an integer, preserving its left-aligned nature.

### Implementation Considerations {#implcons}


All floating-point numbers, including zeros and infinities, are signed.
A NaN also carries a sign bit.
Each of the three formats binary16, binary32, and binary64 define a
fixed assignment of bits in the representation towards the sign bit,
an exponent, and a "significand" (which represents the mantissa, with
details sometimes depending on the specific exponent value).

| Format   | Sign bit | Exponent | Significand |
| binary16 |        1 |        5 |          10 |
| binary32 |        1 |        8 |          23 |
| binary64 |        1 |       11 |          52 |
{: #tab-bits title="Bit Allocation in Floating Point Formats" }

Infinite numbers are represented in each format choice with a sign
bit, the highest available exponent value (all ones) and all-zero
significand.
NaN values are represented with a sign bit, the highest available
exponent value (all ones) and a non-zero significand, which carries a
leading quiet bit with the rest of the bits allocated to the NaN payload.

To qualify as a generic encoder or decoder, a CBOR library needs to
implement as much of {{IEEE754}} support as reasonably possible on the
platform it addresses.
What is reasonably possible depends on:

* platform support for {{IEEE754}} numbers.  If there is no such
  support, the generic decoder may need to resort to offering the
  interchanged value to the application, suitably tagged.
* If there is partial support, it may be harder to find a good
  solution.  This is specifically a problem for platform support that
  works well in most cases, but exhibits some dark corners.
  E.g., the implementation may support a single NaN value
  consistently, but not preserving NaN information present in the NaN
  values.

Where an implementation needs to convert between different floating
point formats, e.g., because not all formats are fully supported by
the platform, or to implement Preferred Serialization (including for
Common Deterministic Encoding) in an encoder, conversion of NaNs in
these formats is best done by operating on the bit patterns of the
{{IEEE754}} number in the following way:

* Expansion (towards a larger size format):
   * preserve the sign bit
   * expand the (all-ones) exponent to the larger (all-ones) exponent
   * fill up the significand with zero bits on the right
* Contraction (towards a smaller size format):
   * preserve the sign bit
   * truncate the (all-ones) exponent to the smaller (all-ones) exponent
   * truncate the significand from the right; check if the removed
     bits were all zero.

If the contraction is optional, e.g., for Preferred Serialization, do
not perform the contraction if the removed bits in the significand
truncation aren't all zero.
If the contraction is required to fit into limited platform types
(e.g., binary32 only), a failed truncation check should be signaled to
the application.
We say a contraction "preserves the NaN information" if subsequent
expansion to the original size format recreates the same NaN value.

{{app-nan}} gives additional detailed considerations for implementations
that aspire to provide full support for NaNs, preserving NaN information.


# Other Floating Point Numbers {#sec-float}

{{RFC8949@-cbor}} also defines tags 4 and 5 for a representation of
decimal and binary floating point numbers that is not constrained by
the types provided by IEEE 754.
These tags are very flexible, but this flexibility comes with a choice
of ways they could be integrated into a generic encoder.
Because of this flexibility, tags 4 and 5 do not define a Preferred
Serialization or a deterministic encoding.

{{Section 3.2 of ?I-D.ietf-cbor-time-tag}} uses representations derived
from the tags 4 and 5 to represent timestamps.
{{Section 6.1 of ?I-D.ietf-cbor-time-tag}} lists various other tags that
can be used for representing numbers for advanced arithmetic,
including rational numbers in fraction form (tag 30).

# Tagged Arrays of Numbers {#sec-array}

{{-arrays}} defines tags for typed arrays, i.e., arrays of numbers that
all are represented in the same way.
The choices defined in the {{-arrays}} are all based on traditional
platform number representations (unsigned integers, signed integers,
IEEE 754 floating point values) and even come in little-endian and
big-endian variants, often removing the need to convert the numbers
from an internal to an interchange form.
As conversion for interchange is not envisioned,
considerations for a preferred serialization are not applicable.
As the recipient may need a conversion for ingestion of the arrays,
some considerations from {{sec-ieee}} may apply.

# Security Considerations

The general security considerations for representing data in common
data representation formats apply, e.g., those in {{Section 10 of
RFC8949@-cbor}}.

(TODO)

# IANA Considerations

(TODO:

Add nan'' registration when that is ready)

--- back

# Implementers' Checklists for Floating Point Values {#impcheck}

This check list employs {{BCP14}} keywords to indicate interoperability
requirements on implementations.

The following considerations apply to encoding (emitting) floating
point values in a generic encoder:

   * The length of the argument is encoded in the lower 5 bits of the
     first byte ("ai"), which indicates half precision (binary16, ai = 0x19),
     single precision (binary32, ai = 0x1a) and double precision
     (binary64, ai = 0x1b).

     For preferred serialization: if multiple of these encodings
     preserve the precision of the value to be encoded, only the
     shortest form of these MUST be emitted.
     That implies that encoders MUST support half-precision and (if
     there is support for more than half precision on the platform)
     single-precision floating point.
     Positive and negative infinity and zero MUST be represented in
     half-precision floating point.

   * NaNs MUST be supported, for all values of NaN information allowed
     in {{IEEE754}}.

     As with all floating point numbers, NaNs with payloads MUST be
     contracted to the shortest of double, single or half precision that
     preserves the NaN information.

     The reduction is performed by removing the rightmost N bits of the
     payload, where N is the difference in the number of bits in the
     significand (mantissa) between the original format and the
     reduced format.
     The reduction is performed only (preserves the value only) if all the
     rightmost bits removed are zero.
     (This will always reduce a double or single quiet NaN with an
     otherwise zero NaN payload, which is typically what is returned
     from an operation such as 0.0/0.0, to a half-precision quiet NaN
     encoded as 0xf9 7e00.)

The following considerations apply to decoding (ingesting) floating
point values in a generic decoder that supports IEEE 754 floating-point numbers:

   * Half-precision values MUST be accepted.
   * Double- and single-precision values SHOULD be accepted; leaving these out
     is only foreseen for decoders that need to work in exceptionally
     constrained environments.
   * If double-precision values are accepted, single-precision values
     MUST be accepted.
   * NaNs, MUST be accepted, preserving the NaN information for use of
     the application.


## NaN Payloads {#app-nan}

An IEEE-754 data item has up to 52 bits in the significand.
For a NaN, the first of these bits is used to indicate whether the NaN
is signalling (0) or quiet (1).
The up to 51 bits in the rest of the significand are called the "NAN
payload".

The payload’s original purpose is diagnostic information to explain
why a NaN was generated by a local computation.
There is no standard for the contents of a NaN payload.

CBOR allows NaNs with non-zero payloads to be encoded.
(Due to the way infinite numbers are encoded in {{IEEE754}},
zero-payload NaN always must be quiet NaNs.)

As a result, if a protocol design does not use NaNs with non-zero
payloads and is using preferred serialization then NaN must be encoded
as a half-precision with the quiet bit set and the payload set as 0,
specifically 0xF97E00.
If a design does not use NaNs with non-zero payloads and preferred
serialization is not used, then the single and double precision quiet
NaNs, 0xFA7FC00000 and 0xFB7FF0000000000000, may also be used.

NaN payloads have been in the IEEE-754 standard since 2008, but
programming environments often still do not provide facilities (e.g.,
APIs) to make use of them.
For example, in C there is the isnan() API to check if a value is a
NaN, but there are no APIs to construct or access the NaN payload.
The typical way to work with a NaN payload is to reinterpret the
floating-point value as an unsigned integer and then use shifts and
masks to unpack the IEEE-754 representation.

### NaN Implementation Details

This section is primarily for CBOR library implementors.

CBOR attempts to limit the MUSTs about CBOR implementations in order
to allow its use in a large variety of constrained use cases.
For example, support for integers is not required because a protocol
might need only strings.
Similarly, there is no MUST that requires support of NaN and NaNs with
non-zero payloads, but the recommendation here is that any generic
CBOR library that supports floating-point support NaNs, preferably
also with non-zero NaN payloads.

In most environments, there is little extra work to do to support NaN
without payloads if floating-point is supported.
NaNs will usually flow through as any other floating-point value.

Generic CBOR libraries are expected to support preferred serialization
of floating-point including NaNs.
For NaNs with zero payloads, this requires reducing to a half-precision
NaN without a payload.
This requires a few explicit extra lines of code.
See the sample half-precision implementation in Appendix D of RFC 8949.

The implementation of preferred serialization of NaN payloads needs a
few more additional lines.
As with preferred serialization, NaN payloads must be reduced but only
if they can be reduced without the loss of any non-zero payload bits.
Programming platform provided floating-point hardware and software may
or may not do this correctly for double to single conversion.
The sample half-precision implementation in Appendix D of RFC 8949
only supports NaNs without payloads.

A double precision NaN payload contains 51 bits, a single 22 bits and
a half 9 bits, in each case all but the first bit of the significand.
A double precision NaN can be reduced to a single precision NaN only if the right-most 29 payload bits are zero.
A single precision NaN can be reduced to a half precision NaN only if the right-most 13 payload bits are zero.
A double NaN can be reduced to a half precision NaN only if the right-most 42 payload bits are zero.
Note that the exponent is always all-ones for NaN, so this is simpler
than the equivalent contraction of regular, non-NAN, floating-point values.

To implement the above, most CBOR libraries will have to reinterpret
the floating point value as an unsigned integer and use shifts and
masks, based in the internal representation defined in {{IEEE754}}.

Testing on some CPUs has shown them to do this correctly for conversion between single and double.
However, it may not be very useful to rely on platform libraries for the following reasons.
First, they may provide no support at all for half-precision and half-precision is required for preferred serialization.
Second, NaN payloads are a relatively recent and very specialist
feature that is not usually used in interchange.

If platform implementation is relied upon, NaN payload reduction should be tested on each platform.
Open source libraries intended to run on multiple platforms may be
better off not relying on the platform.

### NaN Tests Examples

The IEEE-754 numbers are given as a 64-bit (binary64) or 32-bit
(binary32) unsigned integer in hex to show the bits that make up the
floating-point value.
All of the following are NaNs.

|    IEEE-754 Number |        CBOR Encoding | Comment                                         |
| 0x7ff8000000000000 |             0xf97e00 | qNaN reduced from double to half                |
| 0x7ff8000000000001 | 0xfb7ff8000000000001 | Can't be reduced because of bit set in payload  |
| 0x7ffffc0000000000 |             0xf97fff | 10-bit payload that can be reduced to half      |
| 0x7ff80000000003ff | 0xfb7ff80000000003ff | right-justified payload can't be reduced        |
| 0x7fffffffe0000000 |         0xfa7fffffff | 23-bit payload that reduces to single           |
| 0x7ffffffff0000000 | 0xfb7ffffffff0000000 | 24-bit payload that can't be reduced            |
| 0x7fffffffffffffff | 0xfb7fffffffffffffff | All payload bits set                            |
|         0x7fc00000 |             0xf97e00 | qNaN reduced from single to half                |
|         0x7fffe000 |             0xf97fff | single 10-bit payload that can be reduced       |
|         0x7fbff000 |         0xfa7fbff000 | single payload that can't be reduced to 10 bits |
{: #nan-examples title="Examples for Preferred Serialization of NaN values"}

# Acknowledgments
{:unnumbered}

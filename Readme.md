# About Raetselnasen-Solver
Welcome!

This is an ultra-specialized program written to make solving a certain type of puzzle easier, the puzzle in question being the yearly summer puzzle released on [raetselnasen.de](https://www.raetselnasen.de/) (literal translation: puzzle noses).

Let us first understand how the puzzle works: First, there are about 25 individual riddles, each having a single string as its solution. These *partial solutions* must then be combined to obtain the puzzle's actual solution, which is done as follows:
 * Order all partial solutions by their word value. The value of a word is simply the sum of its letter values, and the value of a letter is its position in the alphabet (i.e. ``a = 1, b = 2, ...``).
 * The final solution can then be built by combining the letters of the partial solutions according to a list of simple rules. Each rule specifies a letter position (either the *i*-th letter from the front, the *i*-th letter from the back or the middle letter) of the *j*-th word in the ordered word list.

Since the final solution should be some combination of actual words, it might be possible to guess it even with several unknown partial solutions.
Because the position of each partial solution is not fixed (we do not know the word values of the unknown partial solutions, only the *order* of the known partial solutions), there are quite a lot of possible arrangements even when only a few partial solutions are unknown.

This is where this handy program comes in: It iterates through all possible arrangements of letters in the final solutions and checks whether words in the dictionary exist to fill the gaps.

# Running this Program
This program is written in ``C++`` and should be compatible with ``cmake 3.28.1`` or greater and any compiler providing ``C++ 11`` features (locally I've been compiling it with ``g++ 13.1.0``). It takes the following command line options:
 * ``-f <puzzle_config>``: This is the most important argument and points to the configuration file the program should be using (more on the config file format in the next chapter). Please note that all paths specified in the config file are relative to the program's working directory, not the config file itself
 * ``-t <dictionary_name>``: Instead of solving the supplied puzzle, enters a test mode for the specified dictionary.
 * ``-q``: Instead of solving the supplied puzzle, enters a query test mode for the puzzle.

# Crafting a ``puzzle_config``-File
A puzzle config file specifies all the options needed to solve the puzzle. There are three types of objects (``general, dictionary, word``) that can be defined in the config file. The syntax is 
```
object1 {
    property = value;
    list_property = [v1;v2;v3]
}
object2 {
    ...
}
```
Special characters are `=`, `;`, brackets `[]` and paranthesis `{}`. A semicolon is generally needed to finish a property declaration or to separate list elements, however its use is optional when it would *follow* another special character.
 * ``general`` contains general options for the puzzle. These include 
   * ``total_words (int)``: The number of words in the final solution
   * ``min_matches (int)``: How many words of the final solution should match at minimum for a configuration to not be discarded
   * ``show_matches {true | false}``: Indicates whether the program should show the matching words for each successful query (this can grow very fast very quickly)
   * ``show_config {true | false}``: Whether to show the positions of the unknown words in the sorted word list for each successful query configuration
   * ``words_file (string)``: The path of the file containing the partial solutions (separated by new lines) (relative to the working directory of the program itself, not this file!). The program will ignore lines that start with a `#`, and (in the case of uncertainty) multiple partial solutions for the same slot can be separated by `,` (Think of the riddle: The better of the two sci-fi franchises with possible solutions `startrek` and `starwars`). The program will generate all possible combinations of partial solutions and solve for each individually.
 * ``dictionary`` defines a dictionary that can be used by the words of the final solution and contains the following options:
   * ``filename (str)``: The path of the file containing all dictionary words (separated by new lines)
   * ``name (str)``: How we can refer to this dictionary in ``word`` definitions
   * ``type {single_word | multi_word | anagram}``: The dictionary type. ``single_word`` allows normal word lookup, ``multi_word`` also accepts concatenations of words in the dictionary file (very important in the German language ;)) and finally ``anagram`` also finds anagrams of (single) words
   * ``filters [Filter]``: A list of filters that should be applied to the words before loading them into the dictionary. A ``Filter`` has two properties: a ``regex (str)`` that each word is matched against and ``satisfy {true | false}`` which indicates whether words should be kept or discarded when they satisfy the regex.  
   All filters are applied to every word.
 * ``word`` defines a word in the actual solution and has the following properties:
   * ``dictionary (string)``: The name of the dictionary that should be used for lookups
   * ``letters [LetterRule]``: The letter rules by which this word is constructed. Each ``LetterRule`` is of the following syntax ``[F|B|M]<index>P<index>`` where the first letter specifies whether the letter should be taken from the **F**ront, **B**ack or **M**iddle along with the (for ``M``-type rules unnecessary) index (starting from 1), followed by ``P`` and the index of the word in the ordered partial solution list from which the letter should be taken.
   * ``filters [Filter]``: A list of filters, which work exactly the same as the dictionary filters, only they are applied to the dictionary query created for this word.

# A Simple Example
The folder ``./simple_test`` contains a simple example to better understand the workings of the program better. There are seven partial solutions
``bud this program solves puzzles flawlessly`` and the final solution consists of three words:
 * The first word is constructed with the letter rules ``letters = [B3P2;B2P5;F5P6;F3P5;F2P3]`` and is a normal word.
 * The second word is a concatenation of at least two words and its letter rules are ``letters = [F1P6;B6P4;F3P4;F1P1;F2P7;B5P7;B1P4;F1P3;B5P5;B2P7;F4P5;F3P2;B2P3;MP4]``
 * The last word is anagrammed and consists of the letters ``[F2P7;B4P3;F4P7;B3P4;B1P3]``.

Our word bank (a list of 10'000 (popular?) english words yoinked from [here](https://apiacoa.org/publications/teaching/datasets/google-10000-english.txt)) is saved in ``dictionary.txt`` and the partial solutions are contained in ``words.txt``. We may craft the following configuration file:
```
general {
    min_matches = 3;
    total_words = 7;
    show_matches = true;
    words_file = simple_test/words.txt;
}
dictionary {
    name = simpledict;
    filename = simple_test/dictionary.txt;
    type = single_word;
}
dictionary {
    name = anagramdict;
    filename = simple_test/dictionary.txt;
    type = anagram;
}
dictionary {
    name = multidict;
    filename = simple_test/dictionary.txt;
    type = multi_word;
    filters = [{satisfy=true;regex=....+;}];
}
word {
    letters = [B3P2;B2P5;F5P6;F3P5;F2P3];
    dictionary = simpledict;
}
word {
    letters = [F1P6;B6P4;F3P4;F1P1;F2P7;B5P7;B1P4;F1P3;B5P5;B2P7;F4P5;F3P2;B2P3;MP4]
    dictionary = multidict;
}
word {
    letters = [F2P7;B4P3;F4P7;B3P4;B1P3]
    dictionary = anagramdict;
}
```

Since random dictionary files generated from the internet are often not very clean, it is very important to preprocess them as otherwise the number of matches (especially in ``multi_word``-type dictionaries) can increase dramatically. As a simple clean-up, we remove all words from the ``multi_word`` dictionary of length less than four.

Depending on how many of the seven partial solutions we provide in ``words.txt``, the outputs may look drastically different.

For example, providing only ``bud this sound flawlessly``, we generate the following output:
```
...
2 orderings matching at least 3 words were found!
h???o ???ble?s?l?in? low?d
howto enables        would
hello enabled        world
      problem
      problems
          lease
          least
          lens
          less
          legs
          lets
             selling
             solving
              flying
              alpine
              plains
               lying
h?l?o f??b???s???in? ?o??d
hello forbes         could
            usps     cloud
            usda     codes
            usgs     condo
            uses     crowd
            user     bored
            used     bound
            espn     order
            msie     blood
            asus     nodes
            asin     video
            asia     decor
            asks     oxide
            assuming older
            isbn     lodge
            isle     dolls
            issn     depot
                zinc moved
                kind doubt
                king audio
                ring found
                line loads
                link floyd
                bind owned
                tiny stood
                wins disco
                wing dover
                wine drove
                wind world
                fine words
                find sword
                sink voted
                sing honda
                hint loved
                mins doors
                mint would
                mine sound
                mini blond
                mind devon
                nine model
                ping flood
                pins boxed
                pine lloyd
                pink foods
                     diego
                     noted
                     modem
                     adopt
                     bonds
                     woods
                     rhode
                     today
                     modes
                     dozen
                     idaho
                     round
                     holds
                     proud
                     goods
                     adobe
                     doing
                     dodge
                     dildo
                     wound
                     donor
                     hoped
                     pound
                     solid
                     roads
                     drops
                     avoid
                     kodak
                     donna
                     radio
                     board
                     broad
```
Obviously we cannot say with certainty what the solution should be, but depending on the context we may be able to discard the second group of matches, since ``forbes`` might be very unlikely to be part of the solution.

Adding the filter ``filters=[{satisfy=false;regex=^f.*;}]`` to our second word discards that group of matches completely, and we arrive at
```
...
1 orderings matching at least 3 words were found!
h???o ???ble?s?l?in? low?d
howto enables        would
hello enabled        world
      problem
      problems
          lease
          least
          lens
          less
          legs
          lets
             selling
             solving
              flying
              alpine
              plains
               lying
```
An educated guess might leave us with ``hello problemsolving world`` (which we could have seen without the filter as well, of course).
More sophisticated filtering can also be done externally, however the filters are useful because they reduce the number of queries (or the number of words in the dictionary), therefore making the program finish much more quickly.

In addition, during query iteration, it is only checked whether words satisfying a given query exist, which is usually much faster than compiling a list of *every* word that might fit.

# A more Realistic Example
While the above example might look a bit contrived, we can validate our program's performance with [last year's puzzle](https://www.raetselnasen.de/sommerraetsel-2024/).

A realistic word list for an experienced puzzler may contain 22 of the 27 partial solutions with some uncertainty included:
```
aerztin
augsburg
augustine
byor,bjlb
deutschland
dreihundertvierzehn
faehnleinfieselschweif,fieselschweif
edwardskrone
tieftauchtopf
lalitkumar,kumar
mairedecolmar,maire
panikpraesident
rodriguez
gotik
saechsisch
schoenerwohnen
sitzaltenburg
trossingen
redewendungen
vier
clinteastwood,clintoneastwoodjr
sodoge
```
Our puzzle configuration file is not that much longer than before, you can find it in ``puzzle24/puzzle_config.txt``. Of course, the multiword dictionary is already used for the right words, but with a bit of trial and error you can definitely arrive at this output.
Setting ``show_matches=false;`` to declutter the output a bit, we are left with an already pretty promising output:
```
...
89 orderings matching at least 5 words were found!
mon???ico zuzug kin?ablnd zehnhoch?euf ?onn m?h?bv?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug kin?abend zehnhoch?euf ?onn m?h?rb?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug kinoablnd zehnhoch?eu? ?onn m???bviii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug kinoabend zehnhoch?eu? ?onn m???rbiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug kin?ab?nd zehnhoch?euf ?onn m?hl?v?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug kin?ab?nd zehnhoch?euf ?onn m?he?b?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug kinoab?nd zehnhoch?eu? ?onn m??l?viii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug kinoab?nd zehnhoch?eu? ?onn m??e?biii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug kin?abend zehnhoch?euf ?onn m?hlr??ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug kin?abond zehnhoch?euf ?onn m?her??ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug kinoabend zehnhoch?eu? ?onn m??lr?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug kinoabond zehnhoch?eu? ?onn m??er?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon?e?ic? zuzui rino?brnd zehnhoth?eu? ?onn ???le?iii zeugin
+++++++++ ----- --------- ++++++++++++ ++++ +++++++++ ++++++
mon???ico zuzug ?in?abend zehnhoch?euf ?onn m?hlrg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug ?in?abond zehnhoch?euf ?onn m?herg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug ?inoabend zehnhoch?eu? ?onn m??lrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug ?inoabond zehnhoch?eu? ?onn m??ergiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzu? ein?abend zehnhoch?euf ?onn m?hlrg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzu? ein?abond zehnhoch?euf ?onn m?herg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzu? einoabend zehnhoch?eu? ?onn m??lrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzu? einoabond zehnhoch?eu? ?onn m??ergiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on?e?ic? zuzu? kinoabrnd zehnho?h?eu? ?onn m??leviii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on?e?ic? zuzu? kinoabend zehnho?h?eu? ?onn m??rrbiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on?e?ic? zuzu? rinoabend zehnho?h?eu? ?onn m??lrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on?e?ic? zuzu? rinoabond zehnho?h?eu? ?onn m??ergiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon?e?ic? zuzu? kino?brnd zehnhoth?eu? ?onn ???leviii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon?e?ic? zuzu? kino?bend zehnhoth?eu? ?onn ???rrbiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon?e?ic? ?uzu? kinoobrnd zehnhoth?eu? ??nn u??leviii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon?e?ic? ?uzu? kinoobend zehnhoth?eu? ??nn u??rrbiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ico zuzug rin?abond zehnho?h?euf ?onn m?herg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ico zuzug rin?abend zehnho?h?euf ?onn m?hlrg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ico zuzug kin?abend zehnho?h?euf ?onn m?hrrb?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ico zuzug kin?abrnd zehnho?h?euf ?onn m?hlev?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ic? zuzug kinoabrnd zehnho?h?eu? ?onn m??leviii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ic? zuzug kinoabend zehnho?h?eu? ?onn m??rrbiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ic? zuzug rinoabend zehnho?h?eu? ?onn m??lrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on???ic? zuzug rinoabond zehnho?h?eu? ?onn m??ergiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug rin??bond zehnhoch?euf ?onn ??herg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico zuzug rin??bend zehnhoch?euf ?onn ??hlrg?ii ze?gin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon???ic? zuzug kino?brnd zehnhoth?eu? ?onn ???leviii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon???ic? zuzug kino?bend zehnhoth?eu? ?onn ???rrbiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug rino?bend zehnhoch?eu? ?onn ???lrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? zuzug rino?bond zehnhoch?eu? ?onn ???ergiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico ?uzug rin?obond zehnhoch?euf ??nn u?herg?ii ze??in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ico ?uzug rin?obend zehnhoch?euf ??nn u?hlrg?ii ze??in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon???ic? ?uzug kinoobrnd zehnhoth?eu? ??nn u??leviii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
lon???ic? ?uzug kinoobend zehnhoth?eu? ??nn u??rrbiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? ?uzug rinoobend zehnhoch?eu? ??nn u??lrgiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon???ic? ?uzug rinoobond zehnhoch?eu? ??nn u??ergiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug kinoab?nd zehnhoch?eu? bonn m????viii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug kinoabend zehnhoch?eu? bonn m???r?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug ?inoabend zehnhoch?eu? bonn m???rgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzu? einoabend zehnhoch?eu? bonn m???rgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug kinoabrnd zehnho?h?eu? bonn m???eviii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug rinoabend zehnho?h?eu? bonn m???rgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug rino?bend zehnhoch?eu? bonn ????rgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? ?uzug rinoobend zehnhoch?eu? b?nn u???rgiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug kinoab?nd zehnhoch?eu? bonn m??e??iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug ?inoab?nd zehnhoch?eu? bonn m??e?giii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzu? einoab?nd zehnhoch?eu? bonn m??e?giii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug kinoab?nd zehnho?h?eu? bonn m??r?viii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug rinoab?nd zehnho?h?eu? bonn m??e?giii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug rino?b?nd zehnhoch?eu? bonn ???e?giii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? ?uzug rinoob?nd zehnhoch?eu? b?nn u??e?giii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug ?inoabind zehnhoch?eu? bonn m??ek?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzu? einoabind zehnhoch?eu? bonn m??ek?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug kinoabend zehnho?h?eu? bonn m??rr?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug rinoabind zehnho?h?eu? bonn m??ek?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug rino?bind zehnhoch?eu? bonn ???ek?iii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? ?uzug rinoobind zehnhoch?eu? b?nn u??ek?iii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzu? ?inoabind zehnhoch?eu? bonn m??eksiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug ?inoabend zehnho?h?eu? bonn m??rrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug ?inoabind zehnho?h?eu? bonn m??ekkiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzug ?ino?bind zehnhoch?eu? bonn ???ekkiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? ?uzug ?inoobind zehnhoch?eu? b?nn u??ekkiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
ronchlhee ?u?ai ?h?odwig? bnd?t?rereit b?h? oaeeksiml heu?rn
--------- +++++ +++++++++ ++++++++++++ ++++ --------- ++++++
ronchehee ?u?ai ?h?odwig? bnd?t?rereit r?h? oaeoksiml heu?rn
--------- +++++ +++++++++ ++++++++++++ ++++ --------- ++++++
mon??lic? zuzu? ninoabind ?ehnhoch?eu? bonn m??eksiii zeug?n
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzu? einoabend zehnho?h?eu? bonn m??rrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzu? einoabind zehnho?h?eu? bonn m??ekkiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? zuzu? eino?bind zehnhoch?eu? bonn ???ekkiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
mon??lic? ?uzu? einoobind zehnhoch?eu? b?nn u??ekkiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
don??li?? z?zug rinoubend zehnhoc??e?? bonn i??rrgiui zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
don??li?? zuz?g rinoubend zehnhoc??eu? bonn i??rrgi?i zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuz?g rinoubend zehnho?h?eu? bonn i??rrgi?i zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? zuzug rino?bend zehnho?h?eu? bonn ???rrgiii zeugin
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
?on??lic? ?uzug rinoobend zehnho?h?eu? b?nn u??rrgiii zeu?in
+++++++++ +++++ --------- ++++++++++++ ++++ --------- ++++++
rwfchlesg ?ezai e?ng?ben? ?agnh?ririan b?nn ?aerrgemb z?n??e
--------- +++++ +++++++++ ++++++++++++ ++++ --------- ++++++
lonshiis? ?ez?i einoo?ond be?nhotigea? e??n un?ergi?? ?eu?rn
--------- +++++ --------- ++++++++++++ ++++ +++++++++ ++++++
```
We may conclude that some of the words are possibly ``monatlich`` (monthly), ``zuzug`` (immigration / moving *to* a place), ``kinoabend`` (cinema night), which didn't even appear in our dictionary, ``zehnhochneun`` (10^9), ``bonn`` (a city in germany) and ``zeugin`` (witness (female)).

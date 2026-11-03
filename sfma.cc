/*
Creative Commons Legal Code

CC0 1.0 Universal

    CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
    LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
    ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
    INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
    REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
    PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
    THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
    HEREUNDER.

Statement of Purpose

The laws of most jurisdictions throughout the world automatically confer
exclusive Copyright and Related Rights (defined below) upon the creator
and subsequent owner(s) (each and all, an "owner") of an original work of
authorship and/or a database (each, a "Work").

Certain owners wish to permanently relinquish those rights to a Work for
the purpose of contributing to a commons of creative, cultural and
scientific works ("Commons") that the public can reliably and without fear
of later claims of infringement build upon, modify, incorporate in other
works, reuse and redistribute as freely as possible in any form whatsoever
and for any purposes, including without limitation commercial purposes.
These owners may contribute to the Commons to promote the ideal of a free
culture and the further production of creative, cultural and scientific
works, or to gain reputation or greater distribution for their Work in
part through the use and efforts of others.

For these and/or other purposes and motivations, and without any
expectation of additional consideration or compensation, the person
associating CC0 with a Work (the "Affirmer"), to the extent that he or she
is an owner of Copyright and Related Rights in the Work, voluntarily
elects to apply CC0 to the Work and publicly distribute the Work under its
terms, with knowledge of his or her Copyright and Related Rights in the
Work and the meaning and intended legal effect of CC0 on those rights.

1. Copyright and Related Rights. A Work made available under CC0 may be
protected by copyright and related or neighboring rights ("Copyright and
Related Rights"). Copyright and Related Rights include, but are not
limited to, the following:

  i. the right to reproduce, adapt, distribute, perform, display,
     communicate, and translate a Work;
 ii. moral rights retained by the original author(s) and/or performer(s);
iii. publicity and privacy rights pertaining to a person's image or
     likeness depicted in a Work;
 iv. rights protecting against unfair competition in regards to a Work,
     subject to the limitations in paragraph 4(a), below;
  v. rights protecting the extraction, dissemination, use and reuse of data
     in a Work;
 vi. database rights (such as those arising under Directive 96/9/EC of the
     European Parliament and of the Council of 11 March 1996 on the legal
     protection of databases, and under any national implementation
     thereof, including any amended or successor version of such
     directive); and
vii. other similar, equivalent or corresponding rights throughout the
     world based on applicable law or treaty, and any national
     implementations thereof.

2. Waiver. To the greatest extent permitted by, but not in contravention
of, applicable law, Affirmer hereby overtly, fully, permanently,
irrevocably and unconditionally waives, abandons, and surrenders all of
Affirmer's Copyright and Related Rights and associated claims and causes
of action, whether now known or unknown (including existing as well as
future claims and causes of action), in the Work (i) in all territories
worldwide, (ii) for the maximum duration provided by applicable law or
treaty (including future time extensions), (iii) in any current or future
medium and for any number of copies, and (iv) for any purpose whatsoever,
including without limitation commercial, advertising or promotional
purposes (the "Waiver"). Affirmer makes the Waiver for the benefit of each
member of the public at large and to the detriment of Affirmer's heirs and
successors, fully intending that such Waiver shall not be subject to
revocation, rescission, cancellation, termination, or any other legal or
equitable action to disrupt the quiet enjoyment of the Work by the public
as contemplated by Affirmer's express Statement of Purpose.

3. Public License Fallback. Should any part of the Waiver for any reason
be judged legally invalid or ineffective under applicable law, then the
Waiver shall be preserved to the maximum extent permitted taking into
account Affirmer's express Statement of Purpose. In addition, to the
extent the Waiver is so judged Affirmer hereby grants to each affected
person a royalty-free, non transferable, non sublicensable, non exclusive,
irrevocable and unconditional license to exercise Affirmer's Copyright and
Related Rights in the Work (i) in all territories worldwide, (ii) for the
maximum duration provided by applicable law or treaty (including future
time extensions), (iii) in any current or future medium and for any number
of copies, and (iv) for any purpose whatsoever, including without
limitation commercial, advertising or promotional purposes (the
"License"). The License shall be deemed effective as of the date CC0 was
applied by Affirmer to the Work. Should any part of the License for any
reason be judged legally invalid or ineffective under applicable law, such
partial invalidity or ineffectiveness shall not invalidate the remainder
of the License, and in such case Affirmer hereby affirms that he or she
will not (i) exercise any of his or her remaining Copyright and Related
Rights in the Work or (ii) assert any associated claims and causes of
action with respect to the Work, in either case contrary to Affirmer's
express Statement of Purpose.

4. Limitations and Disclaimers.

 a. No trademark or patent rights held by Affirmer are waived, abandoned,
    surrendered, licensed or otherwise affected by this document.
 b. Affirmer offers the Work as-is and makes no representations or
    warranties of any kind concerning the Work, express, implied,
    statutory or otherwise, including without limitation warranties of
    title, merchantability, fitness for a particular purpose, non
    infringement, or the absence of latent or other defects, accuracy, or
    the present or absence of errors, whether or not discoverable, all to
    the greatest extent permissible under applicable law.
 c. Affirmer disclaims responsibility for clearing rights of other persons
    that may apply to the Work or any use thereof, including without
    limitation any person's Copyright and Related Rights in the Work.
    Further, Affirmer disclaims responsibility for obtaining any necessary
    consents, permissions or other rights required for any use of the
    Work.
 d. Affirmer understands and acknowledges that Creative Commons is not a
    party to this document and has no duty or obligation with respect to
    this CC0 or use of the Work.
 */

#include "sfma.hh"

#include <filesystem>
#include    <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>

namespace sfma {
    int SFMA_BLOCK_SIZE = 1;
    namespace { // Annonymous namespace
        constexpr unsigned int KB = 1024;
        constexpr unsigned long long MB = KB * 1024;
        constexpr unsigned long long GB = MB * 1024;
    }

    MemoryFile::MemoryFile(const std::string& path){
        int check_limits = 2;
        int current_checks = 0;

        check:
        current_checks += 1;
        if (std::filesystem::exists(path) and std::filesystem::is_regular_file(path)){
            file_path = path;
            is_valid = true;
            return;
        }

        if (current_checks >= check_limits) throw std::runtime_error("sfma::MemoryFile::MemoryFile(const std::string& path) Error: Could not create the memory file. Invalidating the memory.\n");

        std::ofstream create_file(path); // Create the file if not exists
        goto check;
    }

    Data MemoryFile::get(unsigned long long index){
        if (not is_valid) throw Error::INVALID_MEMORY;
        unsigned long long block_index = index * SFMA_BLOCK_SIZE; // Simple trick to calculate the index in the file in bytes.
        unsigned long long block_ending_index = block_index + SFMA_BLOCK_SIZE - 1;
        unsigned long long file_size = std::filesystem::file_size(file_path);
        if (file_size <= block_index){
            return {};
        }
        else if (file_size > block_index and file_size < (block_ending_index + 1)){
            throw Error::CORRUPTED_BLOCK;
        }

        std::ifstream ifile(file_path, std::ios::binary);
        if (not ifile.is_open()){
            throw Error::MEMORY_FILE_NOT_FOUND;
        }

        ifile.seekg(block_index);
        std::string data_value;
        for (unsigned long long i = 0;i < SFMA_BLOCK_SIZE;i++)
            data_value += static_cast<char>(ifile.get());

        return {true, data_value};
    }

    void MemoryFile::set(unsigned long long index, const std::string& data){
        if (not is_valid) throw Error::INVALID_MEMORY;
        unsigned long long block_index = index * SFMA_BLOCK_SIZE; // Simple trick to calculate the index in the file in bytes.
        unsigned long long block_ending_index = block_index + SFMA_BLOCK_SIZE - 1;
        unsigned long long file_size = std::filesystem::file_size(file_path);
        
        std::string pushable_data;
        for (unsigned long long i = 0;i < data.length();i++){
            if (i < SFMA_BLOCK_SIZE) pushable_data += data[i];
            else break;
        }
        while (pushable_data.length() < SFMA_BLOCK_SIZE)
            pushable_data += '\0';

        
        data_check:
        if (file_size == block_index){
            std::fstream write_file(file_path, std::ios::binary | std::ios::app);
            write_file<<pushable_data;
        }
        else if (file_size < block_index){
            unsigned long long set_buffer_size = 0;
            if (block_index > MB){
                set_buffer_size = buffer_size * MB;
            }
            else if (block_index > KB){
                set_buffer_size = buffer_size * KB;
            }
            else if (block_index < KB){ // Set in bytes
                set_buffer_size = buffer_size;
            }

            std::string null_bytes;
            for (unsigned long long i = 0;i < set_buffer_size;i++){
                null_bytes += '\0';
            }

            std::fstream write_file(file_path, std::ios::binary | std::ios::app);
            while (file_size < block_ending_index + 1){
                if ((file_size + null_bytes.length()) > block_ending_index + 1){
                    set_buffer_size = (block_ending_index + 1) - file_size;
                    null_bytes.clear();
                    for (unsigned long long i = 0;i < set_buffer_size;i++){
                        null_bytes += '\0';
                    }
                }
                file_size += null_bytes.length();
                write_file << null_bytes;
            }
            goto data_check;
        }
        else if (file_size >= block_ending_index+1){
            std::fstream file(file_path, std::ios::binary | std::ios::in | std::ios::out);
            file.seekp(block_index);
            for (unsigned long long i = 0;i < SFMA_BLOCK_SIZE;i++){
                file.put(pushable_data[i]);
            }
            file.flush();
            // file.write(pushable_data.c_str(), pushable_data.length());
            return;
        }
        else if (file_size > block_index and file_size < (block_ending_index + 1)){
            std::fstream file(file_path, std::ios::binary | std::ios::in | std::ios::out);
            file.seekp(block_index);
            file.write(pushable_data.data(), pushable_data.length());
        }
    }

    MemoryFile::~MemoryFile(){
        file_path.clear();
        is_valid = false;
    }

}

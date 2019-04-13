-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2018 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Daniel Stepan (xstepa60@stud.fit.vutbr.cz)
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='1') / zapis do pameti (DATA_RDWR='0')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

	type fsm_state is (
		s_idle, -- pocatecni stav
		s_decode, -- stav pro dekodovani
		s_inc_ptr, -- inkrementace PTR
		s_dec_ptr, -- dekrementace PTR
		s_inc_val, -- inkrementace hodnoty bunky (*ptr)
		s_inc_val2,
		s_dec_val, -- dekrementace hodnoty bunky (*ptr)
		s_dec_val2,
		s_start_loop, -- zacatek cyklu
		s_start_loop2, 
		s_start_loop3,
		s_start_loop4,
		s_end_loop, -- konec cyklu
		s_end_loop2,
		s_end_loop3,
		s_end_loop4,
		s_end_loop5,
		s_end_loop6,
		s_put_char,
		s_put_char2,
		s_get_char,
		s_get_chat2,
		s_comment, -- zacatek komentare
		s_comment2,
		s_comment3,
		s_hex_num, -- stav pro hexadecimalni cisla 0-9
		s_hex_num_letter, -- stav pro hexadecimalni cisla A-F
		s_null, -- zastavi vykonavani programu - return
		s_others -- neznamy prikaz
	);


	signal nstate : fsm_state; -- next state
	signal pstate : fsm_state; -- present state

 -- zde dopiste potrebne deklarace signalu

	-- PROGRAM COUNTER - PC
       signal pc_reg : std_logic_vector(11 downto 0); -- pamet ROM ma velikost 4kB = 2^12 bitu 
       signal pc_inc : std_logic; -- inkrementace PC
       signal pc_dec : std_logic; -- dekrementace PC

       	-- POINTER - PTR - Ukazatel do pameti dat
       signal ptr_reg : std_logic_vector(9 downto 0); -- pamet RAM ma velikost 1kB = 2^10 bitu
       signal ptr_inc : std_logic; -- inkrementace PTR
       signal ptr_dec : std_logic; -- dekrementace PTR

       	-- COUNTER - CNT
       signal cnt_reg : std_logic_vector(7 downto 0);
       signal cnt_inc : std_logic; -- inkrementace CNT
       signal cnt_dec : std_logic; -- dekrementace CNT

       -- MULTIPLEXOR - MX / SEL
       signal multiplexor : std_logic_vector(1 downto 0);


       signal tmp : std_logic_vector(7 downto 0);
begin

 ---------- MULTIPLEXOR 
	mx : process (IN_DATA, DATA_RDATA, multiplexor, tmp)
	begin
		case multiplexor is
			when "00" => DATA_WDATA <= IN_DATA;
			when "01" => DATA_WDATA <= DATA_RDATA+1;
			when "10" => DATA_WDATA <= DATA_RDATA-1;
			when "11" => DATA_WDATA <= tmp;
			when others =>
		end case;
	end process;

 ----------- PC REGISTER
	pc_counter : process (RESET, CLK)
	begin
		if (RESET='1') then -- pri RESETu se vynuluje
			pc_reg <= (others=>'0');
		elsif (CLK'event) and (CLK='1') then
			if(pc_inc='1') then -- pri signalu pc_inc se hodnota registru inkrementuje
				pc_reg <= pc_reg + 1;
			elsif (pc_dec='1') then -- analogicky se hodnota dekrementuje
				pc_reg <= pc_reg - 1;
			end if;
		end if;
	end process;
	
	CODE_ADDR <= pc_reg; -- nastavi se adresa bunky v ROM

------- PTR REG 
	ptr_counter : process (RESET, CLK)
	begin
		if (RESET='1') then
			ptr_reg <= (others=>'0');
		elsif (CLK'event) and (CLK='1') then
			if(ptr_inc='1') then
				ptr_reg <= ptr_reg + 1;
			elsif (ptr_dec='1') then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;

	DATA_ADDR <= ptr_reg; -- nastavi se adresa konkretni bunky v pameti RAM

------- CNT REG - slouzi pri pocitani s cykly
	cnt_counter : process (RESET, CLK)
	begin
		if (RESET='1') then
			cnt_reg <= (others=>'0');
		elsif (CLK'event) and (CLK='1') then
			if(cnt_inc='1') then
				cnt_reg <= cnt_reg + 1;
			elsif (cnt_dec='1') then
				cnt_reg <= cnt_reg - 1;
			end if;
		end if;
	end process;


------- AUTOMAT
	fsm_pstate : process (RESET, CLK)
	begin 
		if (RESET='1') then
			pstate <= s_idle;
		elsif (CLK'event) and (CLK='1') then
			if (EN='1') then
				pstate <= nstate;
			end if;
		end if;
	end process;

	fsm_nstate : process (pstate, CODE_DATA, OUT_BUSY, DATA_RDATA, IN_VLD, cnt_reg, pstate)
	begin

	-- vynulovani stavu a signalu na zacatku

		nstate <= s_idle;	
		pc_inc <= '0';
		pc_dec <= '0';
		ptr_inc <= '0';
		ptr_dec <= '0';
		cnt_inc <= '0';
		cnt_dec <= '0';

		CODE_EN <= '1';
		DATA_EN <= '0';
		OUT_WE <= '0';
		IN_REQ <= '0';
		multiplexor <= "00";

		DATA_RDWR <= '0';
		

		case pstate is
			-- pocatecni stav
			when s_idle =>
				CODE_EN <= '1'; -- aktivni signal povoleni cinnosti
				nstate <= s_decode;

			-- dekodovani instrukce pomoci switch konstrukce
			when s_decode =>
				-- divame se do CODE_DATA, jake instrukcni slovo se nachazi na adrese CODE_ADDR
				case CODE_DATA is
					-- inkrementace hodnoty ukazatele
					when X"3E" => 
						nstate <= s_inc_ptr;
					-- dekrementace hodnoty ukazatele
					when X"3C" =>
						nstate <= s_dec_ptr;
					-- inkrementace hodnoty aktualni bunky
					when X"2B" =>
						nstate <= s_inc_val;
					-- dekrementace hodnoty aktualni bunky
					when X"2D" =>
						nstate <= s_dec_val;
					-- zacatek smycky
					when X"5B" =>
						nstate <= s_start_loop;
					-- konec smycky
					when X"5D" =>
						nstate <= s_end_loop;
					-- putchar
					when X"2E" =>
						nstate <= s_put_char;
					-- getchar
					when X"2C" =>
						nstate <= s_get_char;
					-- komentar
					when X"23" => 
                         			nstate <= s_comment;
					-- pocatek hexa cislic 0-9
                    			when X"30" => 
                            			nstate <= s_hex_num;
                    			when X"31" => 
                            			nstate <= s_hex_num;
                    			when X"32" => 
                            			nstate <= s_hex_num;
                    			when X"33" => 
                            			nstate <= s_hex_num;
                    			when X"34" => 
                            			nstate <= s_hex_num;
                    			when X"35" => 
                            			nstate <= s_hex_num;
                    			when X"36" => 
                            			nstate <= s_hex_num;
                    			when X"37" => 
                        	    		nstate <= s_hex_num;
                    			when X"38" => 
                            			nstate <= s_hex_num;
                    			when X"39" => 
                            			nstate <= s_hex_num;
					-- pocatek hexa cislic A-F
                    			when X"41" => 
                            			nstate <= s_hex_num_letter;
                    			when X"42" => 
                            			nstate <= s_hex_num_letter;
                    			when X"43" => 
                            			nstate <= s_hex_num_letter;
                    			when X"44" => 
                            			nstate <= s_hex_num_letter;
                    			when X"45" => 
                            			nstate <= s_hex_num_letter;
                    			when X"46" => 
                            			nstate <= s_hex_num_letter;
                    			when X"00" => 
                            			nstate <= s_null;
                    			when others => 
                            			nstate <= s_others;
                		end case;
		
		-- IMPLEMENTACE JEDNOTLIVYCH STAVU KONECNEHO AUTOMATU		
			when s_inc_ptr =>
				ptr_inc <= '1';
				pc_inc <= '1';
				nstate <= s_idle;

			when s_dec_ptr =>
				ptr_dec <= '1';
				pc_inc <= '1';
				nstate <= s_idle;

			when s_inc_val => 
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				nstate <= s_inc_val2;

			when s_inc_val2 => 
				multiplexor <= "01";
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				pc_inc <= '1';
				nstate <= s_idle;

			when s_dec_val =>
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				nstate <= s_dec_val2;

			when s_dec_val2 => 
				multiplexor <= "10";
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				pc_inc <= '1';
				nstate <= s_idle;

			when s_put_char =>
				if(OUT_BUSY = '1') then
					nstate <= s_put_char;
				else
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					nstate <= s_put_char2;
				end if;

			when s_put_char2 =>
				OUT_WE <= '1';
				OUT_DATA <= DATA_RDATA;
				pc_inc <= '1';
				nstate <= s_idle;

			when s_get_char =>
				IN_REQ <= '1';
				if(IN_VLD = '1') then
					multiplexor <= "00";
					DATA_EN <= '1';
					DATA_RDWR <= '0';
					pc_inc <= '1';
					nstate <= s_idle;
				else
					nstate <= s_get_char;
				end if;

			when s_comment =>
				pc_inc <= '1';
				nstate <= s_comment2;

			when s_comment2 =>
				CODE_EN <= '1';
				nstate <= s_comment3;

			when s_comment3 =>
				if(CODE_DATA = X"23") then
					pc_inc <= '1';
					nstate <= s_idle;
				else 
					nstate <= s_comment;
				end if;

			when s_start_loop =>
				pc_inc <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				nstate <= s_start_loop2;

			when s_start_loop2 =>
				if(DATA_RDATA = "00000000") then
					cnt_inc <= '1';
					nstate <= s_start_loop3;
				else
					nstate <= s_idle;
				end if;

			when s_start_loop3 =>
				if(cnt_reg = "00000000") then
					nstate <= s_idle;
				else 
					CODE_EN <= '1';
					nstate <= s_start_loop4;
				end if;

			when s_start_loop4 =>
				if(CODE_DATA = X"5B") then
					cnt_inc <= '1';
				elsif(CODE_DATA = X"5D") then
					cnt_dec <= '1';
				end if;
				pc_inc <= '1';
				nstate <= s_start_loop3;

			when s_end_loop =>
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				nstate <= s_end_loop2;

			when s_end_loop2 =>
				if(DATA_RDATA = "00000000") then
					pc_inc <= '1';
					nstate <= s_idle;
				else
				       	nstate <= s_end_loop3;
				end if;

			when s_end_loop3 =>
				cnt_inc <= '1';
				pc_dec <= '1';
				nstate <= s_end_loop4;

			when s_end_loop4 =>
				if(cnt_reg = "00000000") then
					nstate <= s_idle;
				else 
					CODE_EN <= '1';
					nstate <= s_end_loop5;
				end if;

			when s_end_loop5 =>
				if(CODE_DATA = X"5D") then
					cnt_inc <= '1';
				elsif(CODE_DATA = X"5B") then
					cnt_dec <= '1';
				end if;
				nstate <= s_end_loop6;

			when s_end_loop6 =>
				if(cnt_reg = "00000000") then
					pc_inc <= '1';
				else 
					pc_dec <= '1';
				end if;
				nstate <= s_end_loop4;

			when s_hex_num => 
				DATA_EN <= '1';
				pc_inc <= '1';
				multiplexor <= "11";
				-- vsechna hexa cisla jsou ve tvaru 0xV0 ... je treba pridat v hexa 0 (binarne 0000)
				tmp <= CODE_DATA(3 downto 0) & "0000";
				nstate <= s_idle;

			when s_hex_num_letter =>
				DATA_EN <= '1';
				pc_inc <= '1';
				multiplexor <= "11";
				tmp <= (CODE_DATA(3 downto 0) + std_logic_vector(conv_unsigned(9, tmp'LENGTH))) & "0000";
				nstate <= s_idle;
	
			when s_null =>
				nstate <= s_null;

			when s_others =>
				pc_inc <= '1';
				nstate <= s_idle;

			when others => null;

		end case;
	
	end process;

end behavioral;
 

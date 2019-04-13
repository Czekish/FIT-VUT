library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( 
	SMCLK, RESET : in std_logic; -- hodinovy signal, asynchronni inicializace hodnot
	ROW : out std_logic_vector (0 to 7); -- signal pro vyber radku
	LED : out std_logic_vector (0 to 7)  -- signal pro vyber sloupce
);
end ledc8x8;

architecture main of ledc8x8 is

-- vektor pro prepinani mezi stavy 
signal state_change : std_logic_vector(20 downto 0) := (others => '0'); 
-- clock enable counter
signal ce_cnt : std_logic_vector(11 downto 0) := (others => '0');
-- vektor pro prepinani mezi ctyrmi stavy
signal state : std_logic_vector(1 downto 0) := "00";
-- vektory pro radky a ledky
signal rows, leds : std_logic_vector(7 downto 0);
-- signal clock enable
signal ce : std_logic;

begin 

-- CITAC
ce_counter : process(SMCLK, RESET)
	begin
		if RESET = '1' then
			ce_cnt <= "000000000000";
		-- Reakce na nabeznou hranu
		elsif SMCLK'event and SMCLK = '1' then
			-- Pricte k citaci jednicku
			ce_cnt <= ce_cnt + 1;
			-- Pocitadlo na zmenu stavu
			if state_change = "111111111111111111111" then
				state <= state+1;
				state_change <= (others => '0');
			end if;
			state_change <= state_change+1;
		end if;
	end process ce_counter;
	-- Clock enable nastavi na jednicku, pokud v clock enable counteru jsou same jednicky
	ce <= '1' when ce_cnt = "111111111111" else '0';	

--- ROTACE RADKU
rotation : process(RESET, SMCLK)
	begin
		if RESET = '1' then
			rows <= "10000000";
		-- Reakce na nabeznou hranu a signal clock enable
		elsif SMCLK'event and SMCLK = '1' and ce = '1' then
			-- Posunuti jednicky; & je konkatenace
			rows <= rows(0) & rows(7 downto 1);
		end if;
	end process rotation;

-- DEKODER RADKU A LEDEK
decoder : process(rows, state)
	begin
		if state = "00" then
			case rows is	
				when "10000000" => leds <= "00001111";
				when "01000000" => leds <= "01110111";
				when "00100000" => leds <= "01111011";
				when "00010000" => leds <= "01111011";
				when "00001000" => leds <= "01111011";
				when "00000100" => leds <= "01111011";
				when "00000010" => leds <= "01110111";
				when "00000001" => leds <= "00001111";
				when others => leds <= "11111111";
			end case;

		elsif state = "01" then
			case rows is	
				when "10000000" => leds <= "11111111";
				when "01000000" => leds <= "11111111";
				when "00100000" => leds <= "11111111";
				when "00010000" => leds <= "11111111";
				when "00001000" => leds <= "11111111";
				when "00000100" => leds <= "11111111";
				when "00000010" => leds <= "11111111";
				when "00000001" => leds <= "11111111";
				when others => leds <= "11111111";
			end case;

		elsif state = "10" then
			case rows is	
				when "10000000" => leds <= "11011011";
				when "01000000" => leds <= "11100111";
				when "00100000" => leds <= "11111111";
				when "00010000" => leds <= "11100011";
				when "00001000" => leds <= "11011111";
				when "00000100" => leds <= "11100111";
				when "00000010" => leds <= "11111011";
				when "00000001" => leds <= "11000111";
				when others => leds <= "11111111";
			end case;

		elsif state = "11" then
			case rows is	
				when "10000000" => leds <= "11111111";
				when "01000000" => leds <= "11111111";
				when "00100000" => leds <= "11111111";
				when "00010000" => leds <= "11111111";
				when "00001000" => leds <= "11111111";
				when "00000100" => leds <= "11111111";
				when "00000010" => leds <= "11111111";
				when "00000001" => leds <= "11111111";
				when others => leds <= "11111111";
			end case;

		end if;
	end process decoder;

	LED <= leds;
	ROW <= rows;

end main;

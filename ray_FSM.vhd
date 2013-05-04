library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ray_FSM is
	port (clk 			: in  std_logic;
			control		: in std_logic;
--			reset 		: in std_logic;
			posX 			: in unsigned(31 downto 0);
			posY 			: in  unsigned (31 downto 0);
			countstep 	: in unsigned (31 downto 0);
			colAddrIn 	: in unsigned (9 downto 0);
			rayDirX 		: in signed (31 downto 0);
			rayDirY 		: in signed (31 downto 0);
			isSide		: out std_logic;
			texNum		: out unsigned (3 downto 0);
			texX			: out unsigned (31 downto 0);
			floorX		: out unsigned (31 downto 0);
			floorY		: out unsigned (31 downto 0);
			tmpPosXout  : out unsigned (31 downto 0);
			tmpPosYout  : out unsigned (31 downto 0);
			countout		: out unsigned (31 downto 0);
			line_minus_h: out unsigned (31 downto 0);
			invline     : out unsigned (31 downto 0);
			invdist_out : out unsigned (31 downto 0);
			drawStart   : out unsigned (31 downto 0);
			drawEnd     : out unsigned (31 downto 0);
			colAddrOut  : out unsigned (9 downto 0);
			state_out   : out std_logic_vector(11 downto 0);
			WE          : out std_logic;
			ready 		: out std_logic
);
end ray_FSM;

architecture imp of ray_FSM is

type rom_type is array(0 to 575) of unsigned (3 downto 0);
constant MAP_ROM: rom_type := (
	x"1",x"1",x"1",x"1",x"1",x"1",x"1",x"1",x"1",x"1",x"1",x"4",x"4",x"2",x"4",x"4",x"2",x"4",x"2",x"4",x"4",x"4",x"2",x"4",
	x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"1",x"4",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"4",
	x"1",x"0",x"3",x"3",x"0",x"0",x"0",x"0",x"0",x"1",x"1",x"4",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"2",
	x"1",x"0",x"0",x"3",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"2",
	x"1",x"0",x"3",x"3",x"0",x"0",x"0",x"0",x"0",x"1",x"1",x"4",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"4",
	x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"1",x"4",x"0",x"0",x"0",x"0",x"0",x"2",x"2",x"2",x"0",x"2",x"4",x"2",
	x"1",x"1",x"1",x"1",x"0",x"1",x"1",x"1",x"1",x"1",x"1",x"4",x"4",x"4",x"4",x"4",x"4",x"2",x"0",x"0",x"0",x"0",x"0",x"2",
	x"1",x"1",x"1",x"1",x"0",x"1",x"1",x"1",x"1",x"0",x"1",x"0",x"1",x"0",x"1",x"0",x"1",x"4",x"0",x"4",x"0",x"2",x"0",x"2",
	x"1",x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"1",x"1",x"0",x"1",x"0",x"1",x"0",x"1",x"1",x"2",x"0",x"0",x"0",x"0",x"0",x"2",
	x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"1",x"2",x"0",x"0",x"0",x"0",x"0",x"4",
	x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"1",x"2",x"0",x"2",x"0",x"2",x"0",x"2",
	x"1",x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"1",x"1",x"0",x"1",x"0",x"1",x"0",x"1",x"1",x"2",x"4",x"2",x"0",x"2",x"2",x"2",
	x"1",x"1",x"1",x"1",x"0",x"1",x"1",x"1",x"1",x"1",x"1",x"4",x"0",x"2",x"1",x"4",x"1",x"3",x"3",x"3",x"0",x"3",x"3",x"3",
	x"2",x"2",x"2",x"2",x"0",x"2",x"2",x"2",x"2",x"4",x"2",x"4",x"0",x"0",x"2",x"0",x"2",x"3",x"0",x"0",x"0",x"0",x"0",x"3",
	x"2",x"2",x"0",x"0",x"0",x"0",x"0",x"2",x"2",x"4",x"0",x"0",x"0",x"0",x"0",x"0",x"4",x"3",x"0",x"0",x"0",x"0",x"0",x"3",
	x"2",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"2",x"4",x"0",x"0",x"0",x"0",x"0",x"0",x"4",x"3",x"0",x"0",x"0",x"0",x"0",x"3",
	x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"1",x"4",x"4",x"4",x"4",x"4",x"2",x"0",x"2",x"3",x"3",x"0",x"0",x"0",x"3",x"3",
	x"2",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"2",x"2",x"2",x"1",x"2",x"2",x"2",x"2",x"2",x"0",x"0",x"3",x"0",x"3",x"0",x"3",
	x"2",x"2",x"0",x"0",x"0",x"0",x"0",x"2",x"2",x"2",x"0",x"0",x"0",x"2",x"2",x"0",x"3",x"0",x"3",x"0",x"0",x"0",x"3",x"3",
	x"2",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"2",x"0",x"0",x"0",x"0",x"0",x"2",x"3",x"0",x"3",x"0",x"3",x"0",x"3",x"0",x"3",
	x"1",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"3",
	x"2",x"0",x"0",x"0",x"0",x"0",x"0",x"0",x"2",x"0",x"0",x"0",x"0",x"0",x"2",x"3",x"0",x"3",x"0",x"3",x"0",x"3",x"0",x"3",
	x"2",x"2",x"0",x"0",x"0",x"0",x"0",x"2",x"2",x"2",x"0",x"0",x"0",x"2",x"2",x"0",x"3",x"0",x"3",x"0",x"0",x"0",x"3",x"3",
	x"2",x"2",x"2",x"2",x"1",x"2",x"2",x"2",x"2",x"2",x"2",x"1",x"2",x"2",x"2",x"3",x"3",x"3",x"3",x"3",x"3",x"3",x"3",x"3"
);



type states is (A, B, C, D, E ,F, G, H, I,J,K);
signal state : states := A;

-- loop mod signals
signal controlprev : std_logic := '0';
signal count : unsigned (31 downto 0) := (others => '0');
signal countstep_sig : unsigned (31 downto 0):= x"00035a4b";
signal rayDirX_sig : signed (31 downto 0):= x"00035a4b";
signal rayDirY_sig : signed (31 downto 0 ):= x"fffdd183";
signal rayDirX_calc : signed (31 downto 0):= x"00035a4b";
signal rayDirY_calc : signed (31 downto 0 ):= x"fffdd183";

signal rayPosX : unsigned (31 downto 0):= x"05c00000";
signal rayPosY : unsigned (31 downto 0):= x"02e00000";
signal colAddr : unsigned (9 downto 0):= "0000000001";
	
-- 
signal mapSpot : unsigned (3 downto 0) :=(others => '0');
signal countshift : unsigned (31 downto 0):=(others => '0');
signal lineheight : unsigned(31 downto 0):=(others => '0');
signal invlineheight : unsigned(31 downto 0):=(others => '0');
signal invdist : unsigned(31 downto 0):=(others => '0');
signal remainder_line : unsigned(31 downto 0):=(others => '0');
signal remainder_invline : unsigned(31 downto 0):=(others => '0');
signal remainder_invdist : unsigned(31 downto 0):=(others => '0');
signal inc : unsigned(4 downto 0):=(others => '0');

signal inc_limit1 : unsigned(9 downto 0) := "1111111111";
signal inc_limit2 : unsigned(4 downto 0) := "11111";

signal bitselect : unsigned(31 downto 0):=(others => '0');
signal tmpCount : unsigned (31 downto 0):=(others => '0');
signal tmplineNum : unsigned (31 downto 0):=(others => '0');
signal tmpinvDistNum : unsigned (31 downto 0):=(others => '0');

signal tmpPosX : unsigned (31 downto 0) := (others => '0');
signal tmpPosY : unsigned (31 downto 0) := (others => '0'); 

constant line_numerator : unsigned(31 downto 0) := x"78000000";
constant screenHeight : unsigned(31 downto 0) := x"000001E0";
constant halfscreenHeight : unsigned(31 downto 0) := x"000000F0";
constant invdist_numerator : unsigned(31 downto 0) := x"01000000";



begin

process (clk) --Sequential process	
	
	variable drawStartTmp : unsigned (31 downto 0);
	variable drawEndTmp   : unsigned (31 downto 0);
	variable tmp_rayPosX  : unsigned (31 downto 0);
	variable tmp_rayPosY  : unsigned (31 downto 0);
	variable addr         : unsigned (9 downto 0);
	variable remainder_line_var : unsigned (31 downto 0);
	variable remainder_invline_var : unsigned (31 downto 0);
	variable remainder_invdist_var : unsigned (31 downto 0);
		
begin

--	if reset = '1' then
--			state <= A;
--			controlprev <= '0';
--			count <= x"00000000";
--			countstep_sig <= x"00035a4b";
--			rayDirX_sig  <= x"00035a4b";
--			rayDirY_sig <= x"fffdd183";
--			rayPosX <= x"05c00000"; 
--			rayPosY <= x"02e00000";
--			colAddr <= "0000000001";
--			
--			
--			mapSpot <= x"0";
--			colAddr <= "0000000001";
--			countshift <= (others => '0');
--			lineheight <=(others => '0');
--			invlineheight <=(others => '0');
--			invdist <=(others => '0');
--			remainder_line <=(others => '0');
--			remainder_invline <=(others => '0');
--			remainder_invdist <=(others => '0');
--			inc <=(others => '0');
--			
--			inc_limit1 <= "1111111111";
--			inc_limit2 <= "11111";
--
--			bitselect <=(others => '0');
--			tmpCount <=(others => '0');
--			tmplineNum <=(others => '0');
--			tmpinvDistNum <=(others => '0');
			
			
	--elsif rising_edge(clk) then
	if rising_edge(clk) then
		colAddrOut <= colAddr;	
		controlprev <= control;
		
		case state is
			when A =>
				inc_limit1 <= "1111111111";
				inc_limit2 <= "11111";
				state_out <= "100000000000";
				ready <= '1';
				WE<='0';
				if (controlprev = '0' and control = '1') then
					count <= x"00000000";
					countstep_sig <= countstep;
					rayDirX_sig <= rayDirX;
					rayDirY_sig <= rayDirY;
					rayDirX_calc <= rayDirX;
					rayDirY_calc <= rayDirY;
					rayPosX <= posX;
					rayPosY <= posY;
					tmpPosX <= posX srl 10;
					tmpPosY <= posY srl 10;
					tmp_rayPosX  := unsigned(signed(posX));
					tmp_rayPosY  := unsigned(signed(posY));
					addr := ((tmp_rayPosX (27 downto 22)) & "0000" ) + ((tmp_rayPosX (28 downto 22))& "000") + (tmp_rayPosY (31 downto 22));
					
					if(addr<"1001000000") then
						mapSpot <= MAP_ROM(to_integer(addr));
						state <= B;
					else
						mapSpot <= x"0";
						state <= A;
					end if;
					
					colAddr <= colAddrIn;
					
				else
					state <= A;
				end if;
				
			when B =>
				state_out <= "010000000000";
				ready <= '0';
				WE<='0';
				inc_limit1 <= inc_limit1 - 1;
				if (mapSpot = x"0" and inc_limit1 > "0000000000" ) then
				--if (mapSpot = x"0") then
						count <= count + countstep_sig;
						rayPosX <= unsigned(signed(rayPosX) +	rayDirX_sig);
						rayPosY <= unsigned(signed(rayPosY) + rayDirY_sig);
						
						tmp_rayPosX  := unsigned(signed(rayPosX) +	rayDirX_sig);
						tmp_rayPosY  := unsigned(signed(rayPosY) + rayDirY_sig);
						addr := ((tmp_rayPosX (27 downto 22)) & "0000" ) + ((tmp_rayPosX (28 downto 22))& "000") + (tmp_rayPosY (31 downto 22));
						
						if(addr<"1001000000") then
							mapSpot <= MAP_ROM(to_integer(addr));
						else
							mapSpot <= x"0";
							state <= A;
						end if;
						
						state <= B;
				else
					state <= C;
				end if;
				
			when C =>
				ready <= '0';
				WE<='0';
				state<= D;
				state_out <= "001000000000";
				--decrement variables to increase precision
				countstep_sig <= "0000" & countstep_sig(31 downto 4);
				
				
				-- if negative shift in 1's
				if ( rayDirX_sig(31 downto 31) = "1") then 
					rayDirX_sig <= "1111" & rayDirX_sig (31 downto 4);
				else
					rayDirX_sig <= "0000"  & rayDirX_sig(31 downto 4);
				end if;
				
				if ( rayDirY_sig(31 downto 31) = "1") then 
					rayDirY_sig <= "1111" & rayDirY_sig (31 downto 4);
				else
					rayDirY_sig <= "0000"  & rayDirY_sig(31 downto 4);
				end if;
			
			when D =>
				state_out <= "000100000000";
				ready <= '0';
				WE<='0';
				inc_limit2 <= inc_limit2 - 1;
				
				if(mapSpot = x"0" or inc_limit2 = "00000") then
				--if(mapSpot =x"0") then
					state <= E;
				else					
					count <= count - countstep_sig;
					rayPosX <= unsigned(signed(rayPosX) -	 rayDirX_sig);
					rayPosY <= unsigned(signed(rayPosY) -   rayDirY_sig);
					
					tmp_rayPosX  := unsigned(signed(rayPosX) - rayDirX_sig);
					tmp_rayPosY  := unsigned(signed(rayPosY) - rayDirY_sig);
					addr := ((tmp_rayPosX (27 downto 22)) & "0000" ) + ((tmp_rayPosX (28 downto 22))& "000") + (tmp_rayPosY (31 downto 22));
					
					if(addr<"1001000000") then
						mapSpot <= MAP_ROM(to_integer(addr));
					else
						mapSpot <= x"0";
						state <= A;
					end if;
					
					state <= D;
					
				end if;
			
			when E =>
				state_out <= "000010000000";
				ready <= '0';
				WE<='0';
				count <= count + countstep_sig(31 downto 0);
				rayPosX <= unsigned(signed(rayPosX) +	 rayDirX_sig(31 downto 0));
				rayPosY <= unsigned(signed(rayPosY) +   rayDirY_sig(31 downto 0));
				
				tmp_rayPosX := unsigned(signed(rayPosX) +	 rayDirX_sig(31 downto 0));
				tmp_rayPosY := unsigned(signed(rayPosY) +   rayDirY_sig(31 downto 0));
				addr := ((tmp_rayPosX (27 downto 22)) & "0000" ) + ((tmp_rayPosX (28 downto 22))& "000") + (tmp_rayPosY (31 downto 22));
				
				if(addr<"1001000000") then
					mapSpot <= MAP_ROM(to_integer(addr));
				else
					mapSpot <= x"0";
					state <= A;
				end if;
				
				state <= F; 
				
			when F =>
				state_out <= "000001000000";
				ready <= '0';
				WE<='0';
				inc <= "11111";
				countshift <= "0000000000"	& count(31 downto 10); 
				lineheight <= x"00000000";
				invlineheight <= x"00000000";
				invdist <= x"00000000";
				remainder_line <= x"00000000";
				remainder_invline <= x"00000000";
				remainder_invdist <= x"00000000";
				bitselect <= x"80000000";
				tmplineNum <= line_numerator;
				tmpCount <= count;
				tmpinvDistNum <= invdist_numerator;
				
				state <= G;
			
			when G =>
				state_out <= "000000100000";
				ready <= '0';
				WE<='0';
				
				if (inc =  "00000") then
					state <= H;
				else
					state <= G;
				end if;
					
					
				remainder_line_var := (remainder_line (30 downto 0))& tmplineNum(31 downto 31);
				remainder_invline_var := (remainder_invline(30 downto 0) )& tmpCount(31 downto 31);
				remainder_invdist_var := (remainder_invdist(30 downto 0))& tmpinvDistNum(31 downto 31);
				
				
				tmplineNum <= tmplineNum(30 downto 0) & "0";
				tmpCount <= tmpCount(30 downto 0) & "0";
				tmpinvDistNum <= tmpinvDistNum(30 downto 0) & "0";
				
				bitselect <= "0" & bitselect(31 downto 1); 
				-- lineheight = 
				-- invlineheight = (count/h) --> count / screenHeight
				-- invdist = (1<<24)/(count>>10)  --> invdist_numerator/countshift
				
				if (remainder_line_var >= count) then
					remainder_line <= remainder_line_var - count;
					lineheight <= lineheight + bitselect;
				else
					remainder_line <= remainder_line_var;
				end if;
				
				if (remainder_invline_var >= screenHeight) then
					remainder_invline <= remainder_invline_var - screenHeight;
					invlineheight <= invlineheight + bitselect;
				else
					remainder_invline <= remainder_invline_var;
				end if;
				
				if (remainder_invdist_var >= countshift) then
					remainder_invdist <= remainder_invdist_var - countshift;
					invdist <=invdist + bitselect;
				else
					remainder_invdist <= remainder_invdist_var;
				end if;
				
				inc <= inc - 1;
				
			
			when H =>
			
				state_out <= "000000010000";
				ready <= '0';
				WE<='0';
				drawStartTmp := halfscreenHeight -  ( "0" & lineheight(31 downto 1));
				drawEndTmp   := halfscreenHeight + ( "0" & lineheight(31 downto 1));
				
				if (drawStartTmp >= screenHeight) then
					drawStart <= x"00000000";
				else 
					drawStart <= drawStartTmp;
				end if;
				
				if (drawEndTmp >= screenHeight) then
					drawEnd <= screenHeight -1;
				else 
					drawEnd <= drawEndTmp;
				end if;	
				
				line_minus_h <= lineheight - screenHeight;
				invline <= invlineheight;
				invdist_out <= invdist;
				state <= I;
				
			when I =>
				state_out <= "000000010000";
				ready <= '0';
				WE<='1';
				state <= J;
			
			when J =>
				state_out <= "000000001000";
				ready <= '0';
				WE<='1';
				state <= K;
			
			when K =>
				state_out <= "000000000100";
				ready <= '0';
				WE<='1';
				state <= A;
				
			when others =>
				state_out <= "111111111111";
				state <= A;
				
			end case;
			
	end if;
	
	
end process;


------------------------------------------------------------


process (count,mapSpot, rayPosX, rayPosY, rayDirX_calc, rayDirY_calc, tmpPosX, tmpPosY)
	
	variable mapX : unsigned (31 downto 0);
	variable mapY : unsigned (31 downto 0);
	variable checkSide : std_logic;
	variable wallX : unsigned (31 downto 0);
	variable tmpTexX : unsigned (31 downto 0);
	variable floorXvar : unsigned (31 downto 0);
	variable floorYvar : unsigned (31 downto 0);
	
	
	begin
		
	
		if rayDirX_calc < 0 then
			--mapX := rayPosX + x"00000001";
			mapX := ((rayPosX srl 22) + 1) sll 22;
		else
			mapX := (rayPosX srl 22) sll 22;
		end if;
	
		if rayDirY_calc < 0 then
			mapY := ((rayPosY srl 22) + 1) sll 22;
		else
			mapY := (rayPosY srl 22) sll 22;
		end if;
	
		--Calculate distance of perpendicular ray (oblique distance will give fisheye effect!)
		checkSide := '0';
		if rayDirX_calc > 0 and rayDirY_calc > 0 then
			if (rayPosX - mapX) < (rayPosY - mapY) then
				checkSide := '1';
			end if;
		elsif rayDirX_calc > 0 and rayDirY_calc < 0 then
			if (rayPosX - mapX) < (mapY - rayPosY) then
				checkSide := '1';
			end if;
		elsif rayDirX_calc < 0 and rayDirY_calc > 0 then
			if (mapX - rayPosX) < (rayPosY - mapY) then
				checkSide := '1';
			end if;
		elsif rayDirX_calc < 0 and rayDirY_calc < 0 then
			if (mapX - rayPosX) < (mapY - rayPosY) then
				checkSide := '1';
			end if;
		end if;
		
	
		--texturing calculations
		texNum <= mapSpot - 1;
		if checkSide = '0' then wallX := rayPosX;
		else wallX := rayPosY;
		end if;
	
		wallX := wallX - ((wallX srl 22) sll 22);
		
	
		--x coordinate on the texture
		tmpTexX := wallX srl 16;
		if ((checkSide = '1') and (rayDirX_calc > 0)) or ((checkSide = '0') and (rayDirY_calc < 0)) then
			tmpTexX := 64 - tmpTexX - 1;
		end if;
		
		
		texX <= tmpTexX;
	
		--x, y position of the floor texel at the bottom of the wall\
		if (checkSide = '1') and (rayDirX_calc > 0) then
			floorXvar := (rayPosX srl 22) sll 22;
			floorYvar := ((rayPosY srl 22) sll 22) + wallX;
		elsif (checkSide = '1') and (rayDirX_calc < 0) then
			floorXvar := ((rayPosX srl 22) sll 22) + (x"1" sll 22);
			floorYvar := ((rayPosY srl 22) sll 22) + wallX;
		elsif (checkSide = '0') and (rayDirY_calc > 0) then
			floorXvar := ((rayPosX srl 22) sll 22) + wallX;
			floorYvar := (rayPosY srl 22) sll 22;
		else
			floorXvar := ((rayPosX srl 22) sll 22) + wallX;
			floorYvar := ((rayPosY srl 22) sll 22) + (x"1" sll 22);
		end if;
	
		isSide <= checkSide;
		countout <= count;
		
		floorX <= floorXvar srl 10;
		floorY <= floorYvar srl 10;
		
		tmpPosXout <= tmpPosX;
		tmpPosYout <= tmpPosY;
	
	end process;



end imp;